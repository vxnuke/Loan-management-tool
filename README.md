# Loan-management-tool
A program designed to help keep track of profits and clients when lending money.

# How to compile

gcc -o loan -std=c99 $(mysql_config --cflags) loan.c $(mysql_config --libs)
