#!/bin/bash

# script to convert all .puml files into .png image files

# exit immediately if a command fails
set -e

echo "Generating PNG diagrams..."

# process every .puml file in the current directory
for file in *.puml
do
	# skip if no .puml files exist
	[ -f "$file" ] || continue

	echo " -> $file"

	plantuml -tpng "$file"
done

echo
echo "Done!"
