
### Build images and start the containers 

docker-compose up --build

### Stop the contaners and remove them

docker-compose rm -sf

### Enter the containers as root

docker exec -it --privileged gccsynfl bash  
docker exec -it --privileged  nginxserv bash


### install iptables firewall

apt-get update  
apt-get install iptables


### list the rules
iptables -L -v  
iptables -S  


### for ulogd logging 

apt-get update  
apt-get install ulogd2  
service ulogd2 status   
service ulogd2 start   
service ulogd2 restart     

### configure ulogd2 for iptables logging

see fil: ulogd.conf

### configure iptables for logging incoming ICMP packets and SYN packets on port 80

iptables -A INPUT -m limit --limit 1/s -p icmp -j NFLOG --nflog-group 1 --nflog-prefix "ICMP:"    
iptables -A INPUT -p tcp -m tcp --dport 80 --syn -j NFLOG --nflog-group 1 --nflog-prefix "SYN:" 
iptables-save

### install ps, less

apt-get update   
apt-get install procps   
apt-get install less

### install nano editor
apt-get update   
apt-get install nano   


### dump container as docker image

docker commit <container> <image name>   
docker commit nginxserv nginx_image2    
docker commit gccsynfl gcc_image2    

   
   
####  tcpdump

apt-get update   
apt-get install tcpdump      
tcpdump -D       
tcpdump --list-interfaces      
tcpdump --interface eth0      

  
  
  
