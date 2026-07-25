# ZURVAN Subsystem Specification & Memory Topology

This document details the packed binary layout schemas, Kalman filtering parameters, and mathematical clock tracking loops governing the ZURVAN system layers.

---

## 1. Packed Binary Clock Telemetry (`ClockSnapshot`)
To maximize calculation throughput on standard laptop architectures, clock desynchronization variables are packed into flat, non-padded 25-byte structural records.

```cpp
#pragma pack(push, 1)
struct ClockSnapshot {
    uint64_t local_timestamp_us;  // 64-bit microsecond clock time of the local machine
    uint64_t remote_timestamp_us; // 64-bit reported clock time of the target network peer
    uint32_t round_trip_time_us;  // 32-bit network latency round-trip tracking metric
    float    estimated_drift_us;  // 32-bit floating point value calculating clock offset
    uint8_t  is_synchronized;     // 8-bit boolean operational lock indicator flag (0=No, 1=Locked)
};
#pragma pack(pop)
```

---

## 2. Zero-Dependency Single-Dimension Kalman Filter Loop
To strip away heavy external matrix dependencies, clock desynchronization states are evaluated using a hardcoded, highly optimized single-dimension Kalman filter pipeline.
*   **The Prediction Step:**
    $$\hat{x}_{k \mid k-1} = \hat{x}_{k-1} + \text{Drift Rate} \times \Delta t$$
    $$P_{k \mid k-1} = P_{k-1} + Q$$
*   **The Correction Pass:** Ingests network packet ticks to calculate the measurement innovation variance, updates the Kalman gain ($K$), and adjusts the local clock profile to remove network jitter noise with sub-microsecond precision.
