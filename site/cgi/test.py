#!/usr/bin/python3

import os

print("Content-Type: text/html")
print("")  # ligne vide obligatoire
print("<html><body>")
print("<h1>CGI OK</h1>")
print("<p>Method: %s</p>" % os.getenv("REQUEST_METHOD"))
print("<p>Query: %s</p>" % os.getenv("QUERY_STRING"))
print("</body></html>")
