#!/bin/bash

wc `find . \( -name "*.cpp" -or -name "*.hpp" \) -not -path "./build/*"`
