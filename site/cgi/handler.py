#!/usr/bin/env python3
import os
import subprocess
import urllib.parse

print("Content-Type: text/html\r\n")

query = os.environ.get("QUERY_STRING", "")

process = subprocess.Popen(
    ["./contact", query],
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE
)
out, err = process.communicate()
print(out.decode())
