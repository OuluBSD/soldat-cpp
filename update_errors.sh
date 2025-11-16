#!/usr/bin/env bash
./build.sh  2>&1 |grep "error:" |sort |uniq > errors.txt
head errors.txt

