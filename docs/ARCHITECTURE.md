# ZURVAN Deep Technical Architecture & Mathematical Specification

This engineering manifest details the packed binary layouts, Kalman filtering equations, and least-squares linear regression models governing the ZURVAN decentralized temporal synchronization matrix.

---

## 1. Packed Binary Geometry Format (The .zvn Standard)
Every `.zvn` telemetry stream container file avoids serialization overhead by mapping memory registers straight to disk. Blocks are written at a static size profile using an un-padded compilation footprint to guarantee data locality.

```cpp
#pragma pack(push, 1)
struct ClockSnapshot {
    uint64_t local_timestamp_us;  // 64-bit microsecond monotonic clock time of the local machine
    uint64_t remote_timestamp_us; // 64-bit reported clock timestamp of the network peer
    uint32_t round_trip_time_us;  // 32-bit calculated round-trip latency network delay
    float    estimated_drift_us;  // 32-bit floating point clock offset desynchronization metric
    uint8_t  is_synchronized;     // 8-bit boolean operational lock indicator flag (0=No, 1=Locked)
};
#pragma pack(pop)
```
*Total Footprint Size:* Exactly 25 bytes per structural telemetry record block.

---

## 2. Zero-Dependency Scalar Discrete Kalman Filter Loop
To strip away heavy external matrix processing libraries, ZURVAN isolates tracking vectors using a hardcoded, highly optimized single-dimension Kalman filter pipeline. The system maps the clock offset state ($x_k$) continuously across a microsecond clock spectrum.

### Step 1: Time Update (State and Covariance Prediction)
The engine projects the current clock offset state ($\hat{x}_{k \mid k-1}$) and estimation uncertainty covariance ($P_{k \mid k-1}$) forward based on the last calculated linear drift rate velocity ($r$) and the delta duration ($\Delta t$) elapsed since the last handshake pass:

$$\hat{x}_{k \mid k-1} = \hat{x}_{k-1} + (r \times \Delta t)$$

$$P_{k \mid k-1} = P_{k-1} + Q$$

Where $Q$ represents the environmental system process noise parameter ($0.001\mu\text{s}$ baseline).

### Step 2: Measurement Update (Kalman Gain Correction)
When a live timing packet arriving off network streams yields a raw offset measurement ($z_k$), the engine computes the adaptive Kalman Gain coefficient ($K_k$) against the network latency packet jitter variance noise gate ($R$):

$$K_k = \frac{P_{k \mid k-1}}{P_{k \mid k-1} + R}$$

The filter uses this optimal weighting ratio to smooth away the packet delivery noise, locking down the adjusted true offset estimate ($\hat{x}_k$) and minimizing error covariance for the subsequent execution loop:

$$\hat{x}_k = \hat{x}_{k \mid k-1} + K_k \times (z_k - \hat{x}_{k \mid k-1})$$

$$P_k = (1 - K_k) \times P_{k \mid k-1}$$

---

## 3. Least-Squares Linear Regression Clock Drift Modeller
To dynamically isolate the physical speed at which the local machine's quartz crystal oscillator is naturally gaining or losing time against its peers, ZURVAN implements an in-memory linear regression scanner. It processes the localized timestamp records ($x_i$) and corresponding filtered offsets ($y_i$) over standard statistical matrices.

To shield double floating-point resolution constraints from truncating data precision over long running cycles, every timestamp is normalized against the genesis snapshot tracking mark ($T_0$):

$$x_i = \text{local\_timestamp}_i - T_0$$

The drift velocity acceleration slope coefficient ($m$, scaled as microseconds per second) is derived trustlessly from foundational equations:

$$m = \frac{N \sum (x_i y_i) - (\sum x_i)(\sum y_i)}{N \sum (x_i^2) - (\sum x_i)^2}$$

The resulting slope multiplier feeds back directly into the Kalman time-update loop and powers the future timeline forecasting primitive:

$$\text{Future Offset} = \hat{x}_{\text{current}} + (m \times \Delta t_{\text{horizon}})$$
