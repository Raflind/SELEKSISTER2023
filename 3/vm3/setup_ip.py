#!/usr//bin/env python3

import os
import subprocess
import socket
import ipaddress

INTERFACE = "enp0s3"
GATEWAY   = "192.168.10.1"
RESERVE   = { "192.168.10.102", "192.168.10.103"}

def run_cmd(cmd):
    try:
        subprocess.run(cmd, shell=True, check=True)
    except:
        print(f"Error: {e}")

def is_ip_in_use(ip):
    try:
        neigh_output = subprocess.check_output(["ip", "neigh", "show", ip], text=True)
        if "lladr" in neigh_output and "REACHABLE" in neigh_output:
            return True
    except subprocess.CalledProcessError:
        pass

def dhcp_mode():
    print("\n== IP dari DHCP ==")
    run_cmd(f"sudo dhclient -r {INTERFACE}")
    run_cmd(f"sudo ip addr flush dev {INTERFACE}")
    run_cmd(f"sudo dhclient {INTERFACE}")


def static_mode():
    print("\n == IP STATIC ==")
    while True:
        ip = input("Masukkan IP: ").strip()
        try:
            ipaddress.ip_address(ip)
        except ValueError:
            print("IP tidak Valid")
            continue

        conflict = False
        for reserve in RESERVE:
            if ip == reserve or is_ip_in_use(ip):
                print("IP sudah digunakan")
                conflict = True
                break
        if conflict:
            continue
    
        run_cmd(f"sudo dhclient -r {INTERFACE}")
        run_cmd(f"sudo ip add flush dev {INTERFACE}")
        run_cmd(f"sudo ip addr add {ip}/24 dev {INTERFACE}")
        run_cmd(f"sudo ip route add default via {GATEWAY}")
        break

def main():
    print("Pilih IP")
    print("1. DHCP")
    print("2. Static")
    choice = input().strip()

    if choice == "1":
        dhcp_mode()
    elif choice == "2":
        static_mode()

if __name__ == "__main__":
    main()
