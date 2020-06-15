#!/bin/bash

echo -e "\e[31;1m\nFiles have been cleaned\n\e[0m"
rm -r cmake* simMase *Cache* *Make[Ff]ile* *.o 2>/dev/null
