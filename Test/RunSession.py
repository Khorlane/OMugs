import argparse
import getpass
import re
import socket
import sys
import time
from datetime import datetime
from html import escape
from pathlib import Path


TEST_DIR = Path(__file__).resolve().parent
DEFAULT_HOST = "129.212.241.43"
DEFAULT_PORT = 7373
TIMEOUT = 30
COMMAND_DELAY = 1.0
GAME_PROMPT = rb"(?:^|\n)-?\d+H -?\d+M > "
ANSI_COLOR = re.compile(r"\x1b\[([0-9;]*)m")
ANSI_COLOR_BYTES = re.compile(ANSI_COLOR.pattern.encode("ascii"))
HTML_COLORS = {
    "30": "#767676", "31": "#e74856", "32": "#16c60c", "33": "#f9f1a5",
    "34": "#3b78ff", "35": "#d670d6", "36": "#61d6d6", "37": "#f2f2f2",
}
HTML_HEADER = """<!doctype html>
<html lang="en">
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>OMugs Session Transcript</title>
<style>
  body { background: #121212; color: #e6e6e6; margin: 24px; }
  pre { font: 15px/1.5 Consolas, monospace; white-space: pre-wrap; overflow-wrap: anywhere; }
</style>
<body><pre>"""


def render_html(text, color):
    output = []
    for index, part in enumerate(ANSI_COLOR.split(text)):
        if index % 2:
            for code in part.split(";"):
                if code in ("", "0", "39"):
                    color = None
                elif code in HTML_COLORS:
                    color = HTML_COLORS[code]
        elif part:
            escaped = escape(part)
            output.append(f'<span style="color:{color}">{escaped}</span>' if color else escaped)
    return "".join(output), color


def enable_console_color():
    if not sys.stdout.isatty():
        return False
    if sys.platform != "win32":
        return True

    import ctypes
    import msvcrt
    from ctypes import wintypes

    console = wintypes.HANDLE(msvcrt.get_osfhandle(sys.stdout.fileno()))
    mode = wintypes.DWORD()
    kernel32 = ctypes.windll.kernel32
    if not kernel32.GetConsoleMode(console, ctypes.byref(mode)):
        return False
    processed_output = 0x0001
    virtual_terminal_processing = 0x0004
    return bool(kernel32.SetConsoleMode(console, mode.value | processed_output | virtual_terminal_processing))


class TelnetText:
    def __init__(self):
        self.pending = b""

    def read(self, sock, chunk):
        text = bytearray()
        chunk = self.pending + chunk
        self.pending = b""
        while chunk:
            plain, marker, chunk = chunk.partition(b"\xff")
            text.extend(plain)
            if not marker:
                break
            if not chunk or (0xfb <= chunk[0] <= 0xfe and len(chunk) < 2):
                self.pending = marker + chunk
                break
            verb = chunk[0]
            if verb == 0xff:
                text.append(verb)
            if 0xfb <= verb <= 0xfe:
                # This runner only wants plain text, so refuse all options.
                if verb == 0xfb:
                    sock.sendall(b"\xff\xfe" + chunk[1:2])
                elif verb == 0xfd:
                    sock.sendall(b"\xff\xfc" + chunk[1:2])
                chunk = chunk[2:]
            else:
                chunk = chunk[1:]
        return text


def receive(sock, telnet, record, pattern, description, until_close=False):
    response = bytearray()
    deadline = time.monotonic() + TIMEOUT
    try:
        while True:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise TimeoutError
            sock.settimeout(remaining)
            chunk = sock.recv(4096)
            if not chunk:
                if until_close and re.search(pattern, ANSI_COLOR_BYTES.sub(b"", response)):
                    return
                raise RuntimeError(f"Server disconnected while waiting for {description}.")
            response.extend(telnet.read(sock, chunk))
            if not until_close and re.search(pattern, ANSI_COLOR_BYTES.sub(b"", response)):
                return
    except TimeoutError:
        raise RuntimeError(f"Timed out waiting for {description}.") from None
    finally:
        if response:
            text = response.decode("utf-8", errors="backslashreplace")
            record(re.sub(r"\r+\n", "\n", text))


def run_session(host, port, name, password, commands, record):
    with socket.create_connection((host, port), timeout=TIMEOUT) as sock:
        telnet = TelnetText()

        def send(line, secret=False):
            record(("[password masked]" if secret else line) + "\n")
            sock.sendall((line + "\r\n").encode("utf-8"))

        receive(sock, telnet, record, rb"Create a new character Y-N\?", "the welcome prompt")
        send("n")
        receive(sock, telnet, record, rb"Name\?", "the character name prompt")
        send(name)
        receive(sock, telnet, record, rb"Password\?", "the password prompt")
        send(password, secret=True)
        receive(sock, telnet, record, GAME_PROMPT, "the game prompt after login")

        for index, command in enumerate(commands):
            if index:
                time.sleep(COMMAND_DELAY)
            send(command)
            if command.lower() == "quit":
                receive(sock, telnet, record, rb"Bye Bye!", "quit confirmation and disconnect", until_close=True)
            else:
                receive(sock, telnet, record, GAME_PROMPT, f"the game prompt after {command!r}")


def main(scenario_name="Test01.txt"):
    scenario = TEST_DIR / scenario_name
    commands = [line.strip() for line in scenario.read_text(encoding="utf-8-sig").splitlines() if line.strip()]
    if not commands or commands[-1].lower() != "quit" or any(line.lower() == "quit" for line in commands[:-1]):
        raise ValueError(f"{scenario.name} must have one quit command, at the end.")

    print(f"OMugs session test: {scenario.name}")
    host = DEFAULT_HOST
    port = DEFAULT_PORT
    if not 1 <= port <= 65535:
        raise ValueError("The port must be between 1 and 65535.")
    try:
        with (TEST_DIR / "Name.txt").open(encoding="utf-8-sig") as name_file:
            name = name_file.readline().strip()
    except FileNotFoundError:
        name = ""
    if not name:
        name = input("Existing test character: ").strip()
    if not name:
        raise ValueError("Enter an existing character name.")
    password = name

    log_dir = TEST_DIR / "Logs"
    log_dir.mkdir(exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S-%f")
    log_path = log_dir / f"{scenario.stem}-{timestamp}.txt"
    html_path = log_path.with_suffix(".html")
    show_color = enable_console_color()
    html_color = None
    with log_path.open("x", encoding="utf-8") as log, html_path.open("x", encoding="utf-8") as html_log:
        html_log.write(HTML_HEADER)

        def record(text):
            nonlocal html_color
            plain_text = ANSI_COLOR.sub("", text)
            print(text if show_color else plain_text, end="", flush=True)
            log.write(plain_text)
            log.flush()
            fragment, html_color = render_html(text, html_color)
            html_log.write(fragment)
            html_log.flush()

        record(f"{datetime.now().isoformat(timespec='seconds')}  {scenario.name}  {host}:{port}\n")
        try:
            run_session(host, port, name, password, commands, record)
        except (OSError, RuntimeError, KeyboardInterrupt) as error:
            record(f"\nERROR: {error or 'Session interrupted.'}\n")
            return 1
        else:
            record("Session completed. Review the transcript for game behavior.\n")
            return 0
        finally:
            html_log.write("</pre></body></html>\n")
            print(f"\nColor transcript: {html_path}")
            print(f"Plain transcript: {log_path}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run an OMugs test scenario.")
    parser.add_argument("scenario", nargs="?", default="Test01.txt", help="Scenario file (default: Test01.txt)")
    args = parser.parse_args()
    try:
        raise SystemExit(main(args.scenario))
    except (OSError, ValueError, EOFError, KeyboardInterrupt) as error:
        print(f"ERROR: {error or 'Session interrupted.'}")
        raise SystemExit(1) from None
