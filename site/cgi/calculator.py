#!/usr/bin/python3

import cgi
import sys
import os
form = cgi.FieldStorage()

flag1 = False
flag2 = False
n1 = form.getvalue('number1')
op = form.getvalue('operator')
n2 = form.getvalue('number2')
if n1[0] == '-':
	flag1 = True
	n1 = n1[1:]
if n2[0] == '-':
	flag2 = True
	n2 = n2[1:]
for i in n1:
	if i.isdigit() == False:
		print("<h2>Parameters not correct.<h2>")
		exit(1)
for i in n2:
	if i.isdigit() == False:
		print("<h2>Parameters not correct.<h2>")
		exit(1)
if len(op) > 1:
	print("<h2>Parameters not correct.<h2>")
	exit(1)
newn1 = int(n1)
newn2 = int(n2)
if flag1 == True:
	newn1 *= -1
if flag2 == True:
	newn2 *= -1
if op == '+':
	res = newn1 + newn2
elif op == '-':
	res = newn1 - newn2
elif op == '*':
	res = newn1 * newn2
elif op == '/':
	res = newn1 / newn2
print("<h2>%d %s %d = %d</h2>" % (newn1, op, newn2, res))