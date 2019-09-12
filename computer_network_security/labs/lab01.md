## Cisco lab 2.6.1
### Router R1
```
ena
conf t
hostname R1
interface g0/1
ip address 192.168.1.1 255.255.255.0
exit
interface S0/0/0
ip address 10.1.1.1 255.255.255.252
no shutdown
exit
router ospf 1
network 192.168.1.0 0.0.0.255 area 0
network 10.1.1.0 0.0.0.3 area 0
```

### Router R2
```
enable
conf t
hostname R2
interface serial 0/0/0
ip address 10.1.1.2 255.255.255.252
no shutdown
clock rate 64000
exit
interface serial 0/0/1
no shutdown
ip address 10.2.2.2 255.255.255.252
exit
router ospf 1
network 10.1.1.0 0.0.0.3 area 0
network 10.2.2.0 0.0.0.3 area 0
exit
```
### Router R3
```
enable
conf t
hostname R3
interface g0/0
ip address 192.168.3.1 255.255.255.0
no shutdown
exit
interface s0/0/1
ip address 10.2.2.1 255.255.255.252
no shutdown
clock rate 64000
exit
no ip domain-lookup
router ospf 1
network 192.168.3.0 0.0.0.255 area 0
network 10.2.2.0 0.0.0.3 area 0
exit
ospf 1
passive-interface g0/1
show ip ospf neighbor
```

