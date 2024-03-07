from os import system

system("gcc sender.c -o sender")
system("gcc receiver.c -o receiver")

print("Run sender with ./sender")
print("Run receiver with ./receiver")
