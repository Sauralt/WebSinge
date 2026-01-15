#!/usr/bin/python3

import cgi
import sys
import os

form = cgi.FieldStorage()
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')
if first_name == None or last_name == None:
	print("Error 400")
	exit(1)
print("<h2>Hello %s %s</h2>" % (first_name, last_name))
