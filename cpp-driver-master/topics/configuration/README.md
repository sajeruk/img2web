# Configuration

## Load balancing

Load balancing controls how queries are distributed to nodes in a Cassandra
cluster.

Without additional configuration the C/C++ driver defaults to using Datacenter-aware
load balancing with token-aware routing. Meaning the driver will only send
queries to nodes in the local datacenter (for local consistency levels) and
it will use the primary key of queries to route them directly to the nodes where the
corresponding data is located.

### Round-robin Load Balancing

This load balancing policy equally distributes queries across cluster without
consideration of datacenter locality. This should only be used with
Cassandra clusters where all nodes are located in the same datacenter.

### Datacenter-aware Load Balancing

This load balancing policy equally distributes queries to nodes in the local
datacenter. Nodes in remote datacenters are only used when all local nodes are
unavailable. Additionally, remote nodes are only considered when non-local
consistency levels are used or if the driver is configured to use remote nodes
with the [`allow_remote_dcs_for_local_cl`] setting.

```c
/*
 * Use up to 2 remote datacenter nodes for remote consistency levels
 * or when `allow_remote_dcs_for_local_cl` is enabled.
 */
unsigned used_hosts_per_remote_dc = 2;

/* Don't use remote datacenter nodes for local consistency levels */
cass_bool_t allow_remote_dcs_for_local_cl = cass_false;

cass_cluster_set_load_balance_dc_aware(cluster,
                                       used_hosts_per_remote_dc,
                                       allow_remote_dcs_for_local_cl);
```

### Token-aware Routing

Token-aware routing uses the primary key of queries to route requests directly to
the Cassandra nodes where the data is located. Using this policy avoids having
to route requests through an extra coordinator node in the Cassandra cluster. This
can improve query latency and reduce load on the Cassandra nodes. It can be used
in conjunction with other load balancing and routing policies.

```c
/* Enable token-aware routing (this is the default setting) */
cass_cluster_set_token_aware_routing(cluster, cass_true);

/* Disable token-aware routing */
cass_cluster_set_token_aware_routing(cluster, cass_true);
```

### Latency-aware Routing

Latency-aware routing tracks the latency of queries to avoid sending new queries
to poorly performing Cassandra nodes. It can be used in conjunction with other
load balancing and routing policies.

```c
/* Disable latency-aware routing (this is the default setting) */
cass_cluster_set_latency_aware_routing(cluster, cass_false);

/* Enable latency-aware routing */
cass_cluster_set_latency_aware_routing(cluster, cass_true);

/*
 * Configure latency-aware routing settings
 */

/* Up to 2 times the best performing latency is okay */
cass_double_t exclusion_threshold = 2.0;

 /* Use the default scale */
cass_uint64_t scale_ms = 100;

/* Retry a node after 10 seconds even if it was performing poorly before */
cass_uint64_t retry_period_ms = 10000;

/* Find the best performing latency every 100 milliseconds */
cass_uint64_t update_rate_ms = 100;

/* Only consider the average latency of a node after it's been queried 50 times */
cass_uint64_t min_measured = 50;

cass_cluster_set_latency_aware_routing_settings(cluster,
                                                exclusion_threshold,
                                                scale_ms,
                                                retry_period_ms,
                                                update_rate_ms,
                                                min_measured);
```

### Whitelist Filtering

This policy ensures that only hosts from the provided whitelist filter will
ever be used. Any host that is not contained within the whitelist will be
considered ignored and a connection will not be established.  It can be used in
conjunction with other load balancing and routing policies.

NOTE: Using this policy to limit the connections of the driver to a predefined
      set of hosts will defeat the auto-detection features of the driver. If
      the goal is to limit connections to hosts in a local datacenter use
      DC aware in conjunction with the round robin load balancing policy.

```c
/* Set the list of predefined hosts the driver is allowed to connect to */
cass_cluster_set_whitelist_filtering(cluster,
                                     "127.0.0.1, 127.0.0.3, 127.0.0.5");

/* The whitelist can be cleared (and disabled) by using an empty string */
cass_cluster_set_whitelist_filtering(cluster, "");
```


### Connection Heartbeats

To prevent intermediate network devices (routers, switches, etc.) from
disconnecting pooled connections the driver sends a lightweight heartbeat
request (using an [`OPTIONS`] protocol request) periodically. By default the
driver sends a heartbeat every 30 seconds. This can be changed or disabled (0
second interval) using the following:

```c
/* Change the heartbeat interval to 1 minute */
cass_cluster_set_connection_heartbeat_interval(cluster, 60);

/* Disable heartbeat requests */
cass_cluster_set_connection_heartbeat_interval(cluster, 0);
```
Heartbeats are also used to detect unresponsive connections. An idle timeout
setting controls the amount of time a connection is allowed to be without a
successful heartbeat before being terminated and scheduled for reconnection. This
interval can be changed from the default of 60 seconds:

```c
/* Change the idle timeout to 2 minute */
cass_cluster_set_connection_idle_timeout(cluster, 120);
```

It can be disabled by setting the value to a very long timeout or by disabling
heartbeats.

[`allow_remote_dcs_for_local_cl`]: http://datastax.github.io/cpp-driver/api/CassCluster/#1a46b9816129aaa5ab61a1363489dccfd0
[`OPTIONS`]: https://github.com/apache/cassandra/blob/trunk/doc/native_protocol_v3.spec#L278-L282
