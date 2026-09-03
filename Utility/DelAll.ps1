$ErrorActionPreference = "Stop"

$RootDir = "C:\OMugs"
$RunningDir = Join-Path $RootDir "Running"

$TransientDirs = @(
  "Violence\MobPlayer",
  "Violence\PlayerMob",
  "Violence\MobStats\Armor",
  "Violence\MobStats\Attack",
  "Violence\MobStats\Damage",
  "Violence\MobStats\Desc1",
  "Violence\MobStats\ExpPoints",
  "Violence\MobStats\HitPoints",
  "Violence\MobStats\Loot",
  "Violence\MobStats\Room",
  "Control\Events",
  "Control\Mobiles\InWorld",
  "Control\Mobiles\NoMove",
  "Control\Mobiles\Spawn",
  "RoomMob"
)

$TransientFiles = @(
  "Control\RoomMobMove.txt",
  "Control\RoomMobMoveTemp.txt",
  "Control\RoomMobList.txt",
  "Control\RoomMobListTemp.txt"
)

foreach ($RelativeDir in $TransientDirs)
{
  $Dir = Join-Path $RunningDir $RelativeDir
  if (Test-Path -LiteralPath $Dir)
  {
    Write-Host "Cleaning $Dir"
    Get-ChildItem -LiteralPath $Dir -File -Force |
      Where-Object { $_.Name -ne ".gitkeep" } |
      Remove-Item -Force
  }
}

foreach ($RelativeFile in $TransientFiles)
{
  $File = Join-Path $RunningDir $RelativeFile
  if (Test-Path -LiteralPath $File)
  {
    Write-Host "Deleting $File"
    Remove-Item -LiteralPath $File -Force
  }
}
