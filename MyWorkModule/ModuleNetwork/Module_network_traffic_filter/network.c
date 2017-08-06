										/* Модуль ядра с фильтрацией трафика различных пакетов */

#include <net/arp.h>
#include <linux/ip.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/inetdevice.h>
#include <linux/moduleparam.h>

#define ERR(...) printk( KERN_ERR __VA_ARGS__ )
#define LOG(...) printk( KERN_INFO __VA_ARGS__ )
#define DEBUG(...) if( debug > 0 ) printk( KERN_INFO "DBG MSG " __VA_ARGS__ )

static char* link = "enps2n0";							/* имя родительского интерфейса */
module_param( link, charp, 0 );

static char* ifname = "interface_name_this";			/* имя виртуального интерфейса, создаваемого модулем */
module_param( ifname, charp, 0 );

static int debug = 0;									/* для отладки модулей */
module_param( debug, int, 0 );

struct net_device* virt = NULL;

struct priv{											/* структура для различной информации нашего интерфеса, здесь можно размещать любую информацию, размещается в хвосте при создании виртуального интерфейса */
	struct net_device* parent;							/* родительский интерфейс */
	struct net_device_stats stats;						/* статистика виртуального интерфейса, можно занести кол-во отправленных пакетов, байт и т.д. */
	char name_author[30];								/* для примера еще добавил имя автора */
};

static char* strIP( u32 addr ) {     					/* диагностика IP в точечной нотации */
   static char saddr[ MAX_ADDR_LEN ];
   sprintf( saddr, "%d.%d.%d.%d",
            ( addr ) & 0xFF, ( addr >> 8 ) & 0xFF,
            ( addr >> 16 ) & 0xFF, ( addr >> 24 ) & 0xFF
          );

   return saddr;
}

static int network_open( struct net_device* dev ){		/* Вызывается при поднятии сетевого интерфейса(UP) */
	LOG( "Virtual interface %s UP.\n", dev->name );

	struct in_device *in_dev = dev->ip_ptr;
	struct in_ifaddr *ifa = in_dev->ifa_list;

	netif_start_queue( dev );

	return 0;
}

static int network_stop( struct net_device* dev ){			/* Вызывается при DOWN сетевого интерфейса */
	LOG( "Virtual interface %s DOWN.\n", dev->name );

	netif_stop_queue( dev );

	return 0;
}

static struct net_device_stats* network_get_stats( struct net_device *dev ) {		/* Статитистика виртуального сетевого интерфейса */
   return &( netdev_priv( dev )->stats );
}

static netdev_tx_t network_start_xmit( struct sk_buff* skb, struct net_device* dev ){
	++( netdev_priv( dev )->stats.tx_packets );
	( netdev_priv( dev )->stats.tx_packets ) += skb->len;
	skb->dev = ( netdev_priv( dev )->parent );
	skb->priority = 1;

	dev_queue_xmit( skb );													/* send packet */

	return 0;
}

static struct net_device_ops network_function = {
	.ndo_open = network_open,
	.ndo_stop = network_stop,
	.ndo_get_stats = network_get_stats,
	.ndo_start_xmit = network_start_xmit
};

int arp_rcv_pack( struct sk_buff* skb, struct net_device* dev,
				  struct packet_type* pkt, struct net_device* odev ){		/* обработчик пакетов arp */




	kfree_skb( skb );
	
	return skb->len;
}

static struct packet_type arp_proto = {
	.type = __constant_htons( ETH_P_ARP ),
	.dev = NULL,
	.func = arp_rcv_pack,
	.af_packet_priv = ( void* ) 1,
	.list_head = NULL
}; 

int udp_rcv_pack( struct sk_buff* skb, struct net_device* dev,				/* обработчик пакетов udp */
				  struct packet_type* pkt, struct net_device* odev ){




	kfree_skb( skb );
	
	return skb->len;
}

static struct packet_type udp_proto = {
	.type = __constant_htons( ETH_P_UDP ),
	.dev = NULL,
	.func = udp_rcv_pack,
	.af_packet_priv = ( void* ) 1,
	.list_head = NULL
}; 

int tcp_rcv_pack( struct sk_buff* skb, struct net_device* dev,				/* обработчик пакетов tcp */
				  struct packet_type* pkt, struct net_device* odev ){




	kfree_skb( skb );
	
	return skb->len;
}

static struct packet_type tcp_proto = {
	.type = __constant_htons( ETH_P_TCP ),
	.dev = NULL,
	.func = tcp_rcv_pack,
	.af_packet_priv = ( void* ) 1,
	.list_head = NULL
}; 

int ip_v4_rcv_pack( struct sk_buff* skb, struct net_device* dev,			/* обработчик пакетов ip_v4 */
				  struct packet_type* pkt, struct net_device* odev ){


	
	kfree_skb( skb );
	
	return skb->len;
}

static struct packet_type ip_v4_proto = {
	.type = __constant_htons( ETH_P_IP ),
	.dev = NULL,
	.func = ip_v4_rcv_pack,
	.af_packet_priv = ( void* ) 1,
	.list_head = NULL
}; 

void setup( struct net_device *dev ){
      int i = 0;

      ether_setup( dev );

      memset( netdev_priv( dev ), 0, sizeof( struct priv ) );

      dev->netdev_ops = &network_function;
      for( i = 0; i < ETH_ALEN; ++i )
         dev->dev_addr[ i ] = (char)(i + 10 );

}

static int __init network_init( void ){


	LOG( "===== MODULE NETWORK LOADED =====\n" );

	return 0;
}

static void __exit network_exit( void ){




	LOG( "===== MODULE NETWORK UNLOADED =====\n" );
}

module_init( network_init );
module_exit( network_exit );

MODULE_LICENSE( "GPL" );
MODULE_VERSION( "1.2" );
MODULE_AUTHOR( "Vecnik88" );
MODULE_DESCRIPTION( "Filter traffice for the virtual interface" );