#!/usr/bin/env bash
set -euo pipefail

root_dir="/OMugs"
running_dir="$root_dir/Running"

gitkeep_files="$(find "$root_dir" -name ".gitkeep" -type f -print)"
if [ -n "$gitkeep_files" ]
then
  echo "WARNING: .gitkeep files found under $root_dir"
  echo "These are repo scaffolding files and should not be deployed."
  echo "$gitkeep_files"
fi

transient_dirs=(
  "Violence/MobPlayer"
  "Violence/PlayerMob"
  "Violence/MobStats/Armor"
  "Violence/MobStats/Attack"
  "Violence/MobStats/Damage"
  "Violence/MobStats/Desc1"
  "Violence/MobStats/ExpPoints"
  "Violence/MobStats/HitPoints"
  "Violence/MobStats/Loot"
  "Violence/MobStats/Room"
  "Control/Events"
  "Control/Mobiles/InWorld"
  "Control/Mobiles/NoMove"
  "Control/Mobiles/Spawn"
  "RoomMob"
)

transient_files=(
  "Control/RoomMobMove.txt"
  "Control/RoomMobMoveTemp.txt"
  "Control/RoomMobList.txt"
  "Control/RoomMobListTemp.txt"
)

for relative_dir in "${transient_dirs[@]}"
do
  dir="$running_dir/$relative_dir"
  if [ -d "$dir" ]
  then
    echo "Cleaning $dir"
    find "$dir" -maxdepth 1 -type f ! -name ".gitkeep" -delete
  fi
done

for relative_file in "${transient_files[@]}"
do
  file="$running_dir/$relative_file"
  if [ -f "$file" ]
  then
    echo "Deleting $file"
    rm -f "$file"
  fi
done
