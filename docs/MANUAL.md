# ZURVAN Operator Interface & Manual

This manual details the command-line primitives configured to manage the decentralized clock synchronization core.

---

## Clock Telemetry Configuration

### 1. Ingesting Raw Network Timestamp Markers
To input a network peer's clock timing status and calculate the immediate round-trip delay variance, run the `sample` primitive:
```text
zurvan-cli> sample <remote_timestamp_us> <rtt_us>
```

### 2. Executing Kalman Filter Trend Predictions
To compute the future clock drift trajectory based on calculated historical desynchronization speeds, run the `predict` primitive:
```text
zurvan-cli> predict <duration_microseconds_ahead>
```

### 3. Archive Log Serialization
To serialize the active in-memory clock tracking history array back into a dense binary container file, run the `save` primitive:
```text
zurvan-cli> save sync_log.zvn
```
This isolates the output file directly within the `tests/` directory channel.
