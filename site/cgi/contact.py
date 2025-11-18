#!/usr/bin/python3

import cgi
import sys
import os
form = cgi.FieldStorage()

first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')
print("<h2>Hello %s %s</h2>" % (first_name, last_name))
