########################
### values to change ###
########################

save_name = "rt.conf"

_router_name = "tux-rtr5"

# W = 1 in lab I321
# W = 2 in lab I320
_W = 1

# table number
_Y = 3

########################
### values to change ###
########################

template ="""!
version 12.4
service timestamps debug datetime msec
service timestamps log datetime msec
service password-encryption
!
hostname {router_name}
!
boot-start-marker
boot-end-marker
!
! card type command needed for slot/vwic-slot 0/0
logging message-counter syslog
logging buffered 51200 warnings
enable secret 5 $1$u53Q$vBawpP8.1YpCT6ypap1zX.
!
no aaa new-model
ip source-route
!
!
!
!
ip cef
no ip domain lookup
no ipv6 cef
!
multilink bundle-name authenticated
!
!
!
!
!
username root privilege 15 secret 5 $1$8AFR$bNAYevxPFjXFExpnZI2fj.
username cisco password 7 02050D480809
archive
 log config
  hidekeys
! 
!
!
!
!
!
!
!
interface GigabitEthernet0/1
 description $ETH-LAN$$ETH-SW-LAUNCH$$INTF-INFO-FE 0$
 ip address 172.16.{W}.{Y}9 255.255.255.0
 ip nat outside
 ip virtual-reassembly
 duplex auto
 speed auto
!
interface GigabitEthernet0/0
 ip address 172.16.{Y}1.254 255.255.255.0
 ip nat inside
 ip virtual-reassembly
 duplex auto
 speed auto
!
ip forward-protocol nd
ip route 0.0.0.0 0.0.0.0 172.16.{W}.254
ip route 172.16.{Y}0.0 255.255.255.0 172.16.{Y}1.253
ip http server
ip http access-class 23
ip http authentication local
ip http secure-server
ip http timeout-policy idle 60 life 86400 requests 10000
!
!
ip nat pool ovrld 172.16.{W}.{Y}9 172.16.{W}.{Y}9 prefix-length 24
ip nat inside source list 1 pool ovrld overload
!
access-list 1 permit 172.16.{Y}0.0 0.0.0.7
access-list 1 permit 172.16.{Y}1.0 0.0.0.7
!
!
!
!
!
!
control-plane
!
!
!
line con 0
 login local
line aux 0
line vty 0 4
 access-class 23 in
 privilege level 15
 login local
 transport input telnet ssh
line vty 5 15
 access-class 23 in
 privilege level 15
 login local
 transport input telnet ssh
!
scheduler allocate 20000 1000
end
"""

with open(save_name, "w") as f:
    f.write(template.format(router_name = _router_name, W = _W, Y = _Y))
