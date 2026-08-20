# Quick Play over BLE

This is a transport binding of the [Quick Play Protocol](QuickPlay.md) onto Bluetooth Low
Energy. Every feature of the original spec is preserved: the same message model, the same
tags, the same timing and decay rules, the same conflict resolution, the same ability to run
several separated Quick Play networks side by side, and the same principle that each toy keeps
its regular connectivity API. Only the transport changes.

The short version: Quick Play does not need connections. It needs a small, idempotent state
message repeated at a low rate, tolerant of loss, with absence interpreted as zero. That is
precisely what BLE advertising is. So Quick Play over BLE is **connectionless** — devices
broadcast their current output and scan for everyone else's. The Facilitator role disappears
entirely.

## Why broadcast rather than a GATT hub

The obvious translation of "one device becomes the MQTT broker" is "one device becomes a GATT
hub that everyone connects to". That translation is available (see
[Connection Mode](#connection-mode-fallback)) but it is the wrong default, because every
property that made the broker necessary in MQTT is absent from Quick Play:

| Quick Play property                              | What it means for the transport                     |
| ------------------------------------------------ | --------------------------------------------------- |
| Messages are idempotent state, not events         | No ordering, no delivery guarantee, no queue needed  |
| Repeat every second as keep-alive                 | Retransmission is already built into the protocol    |
| Absence of messages means 0.0                     | Loss is self-healing and fails safe                  |
| Rate capped at 5 Hz, not a pattern generator      | Latency budget is hundreds of milliseconds, not tens |
| Highest intensity wins, evaluated per consumer    | Arbitration is receiver-side; no central logic       |
| One topic, everyone sees everything               | No routing, no subscriptions to manage               |

A broker adds a mandatory setup step, a single point of failure, a connection limit, and a
device that must stay powered — in exchange for guarantees Quick Play explicitly does not use.
Broadcasting removes all of that. It also makes the spec's primary use case genuinely practical:
a small battery-powered sensing toy can emit Quick Play for months on a coin cell, which it
could never do while associated to WLAN.

## Topology

There is none to configure. Every participant does one or both of:

- **Emit** — advertise a Quick Play payload carrying its current tag/intensity output.
- **Consume** — scan for Quick Play advertisements, arbitrate, and act.

Devices that do both interleave advertising and scanning; these are independent operations in
the BLE controller and need no coordination. There is no broker, no mDNS service, no
Facilitator, and no election. A session forms the moment two devices are in radio range with a
matching session ID.

Separated Quick Play networks coexist through an explicit 16-bit **Session ID** in the payload
(replacing "which broker did you connect to"). Receivers discard advertisements from other
sessions.

## Wire Format

Quick Play rides in a single Manufacturer Specific Data AD structure inside a legacy
`ADV_NONCONN_IND` (non-connectable, non-scannable) advertisement. Non-connectable advertising
needs no `Flags` AD structure, which leaves the full 31-byte budget for one AD structure.

```
 byte   0  |  AD Length                                          (1)
 byte   1  |  AD Type = 0xFF  Manufacturer Specific Data         (1)
 byte   2  |  Company Identifier, little endian                  (2)
--------------------------- Quick Play payload -----------------------------
 byte   4  |  Magic = 0x51 ('Q')                                 (1)
 byte   5  |  Version = 0x01                                     (1)
 byte   6  |  Session ID, little endian, 0x0000 = open session   (2)
 byte   8  |  Sequence                                           (1)
 byte   9  |  Record, repeated until the end of the AD structure (2+)
```

Budget: 31 − 4 bytes of AD and Company ID overhead = 27 bytes of payload; minus the 5-byte
header leaves 22 bytes, i.e. **up to 11 tag records in a single advertisement**, or 9 records
alongside the optional 4-byte authentication tag. That is far more than any real device needs.

### Records

Each record is one tag and its intensity, and is self-delimiting, so records simply run to the
end of the AD structure and no count field is required.

| Form         | Encoding                                                   | Size     |
| ------------ | ---------------------------------------------------------- | -------- |
| Well-known   | `[tag code (1)][intensity (1)]`                            | 2        |
| Custom       | `[0xFF][name length N (1)][name, UTF-8 (N)][intensity (1)]` | 3 + N    |

| Tag code | Tag         |
| -------- | ----------- |
| `0x01`   | `reward`    |
| `0x02`   | `stimulate` |
| `0x03`   | `signal`    |
| `0x04`   | `motivate`  |
| `0x05`   | `punish`    |
| `0x06`–`0xFE` | Reserved for future well-known tags              |
| `0xFF`   | Custom tag, name follows inline                       |

The five default tags of the original spec cost one byte each; custom tags remain fully
supported and stay human-readable on the wire for anyone sniffing the air, at the cost of their
name length. A device with no custom tags emits a fixed 2-byte record per tag.

### Intensity

`uint8`, in units of 0.5 %, so `0` = 0.0 % and `200` = 100.0 %. Values above 200 are clamped to
200 on receipt, preserving the original rule that intensities outside `[0.0 – 100.0]` are
cropped. Half-percent resolution is finer than any consumer will meaningfully act on and keeps
the value trivially readable.

The original spec says 0.0 must never be sent, because absence is what means zero. That rule
carries over exactly: **a tag at zero is omitted from the advertisement**, it is not sent as an
intensity of 0. This gives a useful two-level zeroing behaviour, described under
[Decay](#decay-and-zeroing).

### Sequence

`uint8`, incremented whenever the record set changes in content — not on keep-alive repeats. It
lets a receiver distinguish a genuinely new state from a repeat (an advertisement is sent on
three channels and may be received several times per interval), detect an emitter restart, and
anchor replay protection when authentication is in use. It wraps freely; receivers compare
modulo 256 within a short window.

### Multiple tags in one packet

The original spec sent one message per tag. Here, an emitter's advertisement carries **all** of
its currently non-zero tags at once. This is a strict improvement: it is one packet instead of
N, it is an atomic snapshot of the emitter's whole output, and it makes a tag returning to zero
visible immediately rather than after a timeout.

## Identity

The advertiser's Bluetooth device address is already in every packet header, outside the 31-byte
payload, so **`origin` costs zero payload bytes**. The address is the source key for conflict
resolution and for allow-listing.

The human-readable name from the original spec (`SmartAssPlug-c4dee2a795e8`) is not needed on the
data path — it is a configuration-time concern. A device that wants to be nameable advertises as
`ADV_SCAN_IND` (scannable, still non-connectable) and returns its `Complete Local Name` in the
scan response, so a UI building a device list can ask for it once without putting a single byte
of it into the intensity stream.

This does require that a participant's address stay stable **for the duration of a session**; a
rotating resolvable private address would fragment one source into many. See
[Security and Privacy](#security-and-privacy) for how to keep both stability and privacy.

## Timing and Radio Parameters

The original spec's timing rules are stated in terms of application-level messages. On BLE the
radio repeats the payload much faster than the application changes it, and that repetition *is*
the keep-alive. Nothing about the observable behaviour changes.

| Original rule                                     | BLE realisation                                                     |
| ------------------------------------------------- | ------------------------------------------------------------------- |
| Repeat roughly every second as keep-alive         | The advertising interval is the keep-alive; no application timer     |
| No more than 5 updates per second                 | The record set may change at most 5 ×/s; radio repeat rate is separate |
| Best practice: message on change, then repeat     | Update the payload and bump `Sequence` on change; the controller repeats |
| Executed for roughly 1.5 s, then falls to 0.0     | Per-source expiry 1500 ms after the last received advertisement      |
| Never emit 0.0                                    | Omit the record                                                      |

Recommended parameters:

| State                       | Advertising interval | Notes                                        |
| --------------------------- | -------------------- | -------------------------------------------- |
| Emitting (any tag non-zero) | 100 – 150 ms         | 0.75 – 1.1 % channel time per emitter        |
| Idle (all tags zero)        | 1000 ms, or stop     | Presence beacon only; consumers already decayed |

Consumers scan passively with a duty cycle of at least 50 % (e.g. 30 ms window / 60 ms interval).

**Sizing rule.** Choose parameters so that a consumer expects at least five receptions inside the
1.5 s decay window, otherwise the decay will chatter on ordinary packet loss:

```
(1.5 s ÷ advertising interval) × scan duty cycle  ≥  5
```

At 150 ms and 50 % duty this gives exactly 5 expected receptions and a total-loss probability
below 0.1 % per window. At 300 ms and 50 % it gives 2.5, which is not enough — either advertise
faster or scan harder. Airtime is cheap: a full-length legacy advertisement is 376 µs, about
1.13 ms per event across all three channels, so ten emitters at 150 ms occupy roughly 7.5 % of
the advertising channels. Congestion is not the limit; scan duty cycle and power are.

## Receiver Behaviour

A consumer keeps one table keyed by `(source address, tag)`:

```
on advertisement received:
    reject unless magic == 0x51 and version is supported
    reject unless session id matches the configured session
    reject unless source is allow-listed (see Security)
    verify authentication tag if the session is authenticated
    for each record:  table[source, tag] = (intensity, now)
    for each tag present in table[source, *] but absent from this packet:
        drop table[source, tag]                       # immediate zero

every control cycle:
    drop every entry older than 1500 ms                # decay
    for each tag:  output[tag] = max(intensity of all live entries for that tag)
```

The last line is the original spec's conflict rule, unchanged: two devices emitting the same tag
resolve to the highest intensity. It was always receiver-side — the broker only ever fanned out —
so removing the broker changes nothing here.

### Decay and zeroing

Two mechanisms, and the difference matters for responsiveness:

- **A tag drops out of an emitter's packet.** The emitter is still present and just stopped
  driving that tag. The consumer zeroes it on the next advertisement, i.e. within roughly one
  advertising interval (~150 ms).
- **An emitter goes silent entirely** — out of range, powered off, crashed. Nothing announces it,
  so every one of its tags expires 1.5 s after its last packet, exactly as in the original spec.

This is a genuine safety property, not merely a convenience: a consumer that loses contact with
its source always falls back to zero on its own, with no disconnect notification, no MQTT
Last Will and Testament, and no dependence on a broker noticing. A toy cannot be left running at
100 % because a link died quietly.

## Configuring a Quick Play Session

The original four steps become three, because there is no Facilitator to appoint:

1. Set the same session code on every device that should play together, or leave them all on the
   open session (`0x0000`). A short passphrase hashed to 16 bits is friendlier than hex.
2. On each consumer, confirm which emitters it accepts, picking them from a scan list. This is a
   one-time pairing gesture and it is what keeps strangers out.
3. Configure the devices' properties — which tags they emit or react to, and how intensity maps
   onto their behaviour — exactly as before.
4. Play along and have fun!

## Security and Privacy

Broadcast is open by construction: anybody in radio range can hear a session and, without
countermeasures, inject into it. MQTT at least had broker credentials, so this needs an explicit
answer. Three layers, in increasing strength:

**Session ID is a selector, not a secret.** It exists so that two nearby sessions do not mix. It
is visible in every packet and must never be treated as access control.

**Allow-listing is the baseline, and is sufficient for most setups.** A consumer acts only on
source addresses the user has explicitly confirmed once, in step 2 above. An attacker cannot
inject without knowing and spoofing an allow-listed address, and there is no way for them to
learn it except by being in range and sniffing. This costs nothing on the wire and fits the
spec's emphasis on simplicity.

**Authentication for hostile environments** — a public play space, a party, anywhere the guest
list is not the room. Derive a session key from the session passphrase with HKDF, and append a
4-byte truncated CMAC over `session id ‖ sequence ‖ records` to the payload (budget: 9 records
instead of 11). This stops injection and tampering outright. Confidentiality, if wanted on top,
is AES-CCM over the record block with the nonce built from the source address and sequence. Note
the residual risk: an 8-bit sequence gives only a short replay window, so a captured packet can
be replayed to re-assert a *recent* state — but never a fabricated one, and never for longer
than the 1.5 s decay allows before the real emitter's next packet overrides it.

**Privacy without losing identity.** Stable addresses make sessions trackable by any passerby,
which for this device class is a real concern. BLE's own privacy feature is the answer: devices
use resolvable private addresses and exchange identity resolving keys during the one-time pairing
in step 2. Paired peers resolve the rotating address back to a stable identity; everyone else
sees an unlinkable address that changes every 15 minutes. Resolution costs one AES operation per
peer per new address, negligible for a handful of peers.

**Consumers always keep their own limits.** Whatever a source asks for, the mapping from
intensity onto device behaviour stays under the consumer's local configuration and safety limits.
That was already the spec's position — intensity mapping "is completely up to the toys'
firmware" — and it is the layer that holds even if every other layer above fails.

Worth noting on the other side of the ledger: BLE's range is roughly a room, and nothing is
exposed to the internet or to the LAN. An attacker has to be physically nearby, which is a
meaningful improvement over a WLAN broker.

## Power

This is where BLE changes what is buildable. The original spec's headline use case is a small
sensing toy driving one or more larger acting toys, and a sensing toy is exactly the device that
cannot afford WLAN.

| Role                       | WLAN + MQTT                              | BLE                                          |
| -------------------------- | ---------------------------------------- | -------------------------------------------- |
| Emit only (sensing toy)    | Association, DHCP, TCP keep-alive, 50–100 mA bursts | Advertise only, never scan; coin-cell viable |
| Consume (acting toy)       | Same, plus broker connection             | Scan duty cycle dominates; usually mains or large cell |
| Broker / Facilitator       | Must stay powered for the whole session  | Does not exist                               |

An emit-only device never scans at all, which is the expensive half of BLE. At a 150 ms
advertising interval while active and 1 s while idle, it is in the microamp-average range.

## Connection Mode (fallback)

Broadcast is the default and should be preferred. A connection-oriented mode is defined for
three cases where broadcast cannot be used: emitting from an iOS app, longer range via a
connection on Coded PHY, and any situation wanting delivery confirmation.

A minimal GATT service, carrying the identical payload so that one parser serves both modes:

| Characteristic | Properties              | Content                                            |
| -------------- | ----------------------- | -------------------------------------------------- |
| Output         | Notify                  | The Quick Play payload, notified on change and ≥1 Hz |
| Input          | Write Without Response  | The Quick Play payload, pushed by a peer            |
| Session        | Read / Write            | Session ID and local name                          |

Semantics are unchanged — the same records, the same decay, the same arbitration — with the
connection's supervision timeout as an additional, faster liveness signal on top of the 1.5 s
decay. The costs are the ones broadcast avoids: a connection limit of a handful of links per
device, an explicit topology to set up, and a device whose disappearance takes its links with it.
Devices should implement broadcast; connection mode is for interoperability, not for the common
path.

## BLE 5 Extended Advertising

On hardware and phones supporting BLE 5 (ESP32-C3/S3/C6/H2 and most phones from 2018 onward;
note that the original ESP32 is Bluetooth 4.2 and cannot do this), extended advertising lifts
three constraints at once:

- **254-byte payloads** — the 31-byte squeeze disappears; unlimited records, full-length custom
  tag names, authentication and encryption without trading away record slots.
- **Coded PHY (S=8)** — roughly 4× the range, at 8× the airtime per packet, so lengthen the
  advertising interval to compensate (a full-length Coded PHY event is ~9 ms across three
  channels; keep it at 300 ms or slower).
- **Multiple advertising sets** — the Quick Play broadcast and the device's own connectable GATT
  interface advertise simultaneously, which is how the original spec's promise that "each toy
  will remain its regular connectivity API" is kept without time-slicing. On Bluetooth 4.2 parts
  the two must share one advertising slot instead.

Because legacy-only scanners cannot see extended advertisements, a device that uses them should
also advertise the compact legacy form as the interoperable baseline.

## Bridging to MQTT

The message model is identical across the two bindings, so a device with both radios can bridge
the BLE air interface and the `quickplay/` MQTT topic with a field-for-field mapping: `origin`
from the source address or its configured name, `tag` from the tag code or inline name,
`intensity` from the half-percent value scaled back to a float. The bridge re-emits each side's
traffic onto the other at the original spec's 1 Hz keep-alive rate and lets its own arbitration
alone — every consumer still resolves conflicts locally, so a bridge that forwards everything
introduces no new semantics.

The one thing a bridge must not do is loop: it tags its own re-emissions with its own origin and
drops anything it recognises as having originated on the other side.

## Platform Constraints

| Platform                    | Emit                                        | Consume |
| --------------------------- | ------------------------------------------- | ------- |
| ESP32 (BT 4.2)              | Yes, legacy adv, one advertising set        | Yes     |
| ESP32-C3/S3/C6/H2 (BLE 5)   | Yes, plus extended adv and Coded PHY        | Yes     |
| Android app                 | Yes, `AdvertiseData.addManufacturerData`    | Yes     |
| iOS app                     | **No** — CoreBluetooth will not advertise manufacturer data from third-party apps; use Connection Mode | Yes |
| Browser / Web Bluetooth     | No                                          | Not reliably; `watchAdvertisements` is limited |

The iOS restriction is the one with real design consequences: an iPhone can listen to a Quick
Play session and can drive devices through Connection Mode, but it cannot be a broadcast emitter.
Any design that assumes a phone as the sensing source needs Connection Mode or a hardware
intermediary.

The Company Identifier should be a project-owned assignment from the Bluetooth SIG. Until one is
held, `0xFFFF` (reserved for internal and development use) works, with the magic byte and version
field filtering out the inevitable collisions with other people's test devices. Shipping on
`0xFFFF` is not recommended.

## Feature Preservation

| Original Quick Play feature                    | Status over BLE                                                    |
| ---------------------------------------------- | ------------------------------------------------------------------ |
| Ad-hoc, no central infrastructure              | Improved — the broker and the Facilitator step are gone             |
| `origin` / `tag` / `intensity` message         | Preserved, binary-encoded; origin is free in the packet header      |
| Five default tags, custom tags allowed         | Preserved; defaults cost 1 byte, custom tags carry their name inline |
| Intensity 0.0 – 100.0, cropped                 | Preserved as 0–200 in 0.5 % units, clamped                          |
| Executed ~1.5 s, then decays to 0.0            | Preserved as a per-source expiry, plus faster per-tag zeroing       |
| Repeat ~1 Hz, cap 5 Hz, message on change      | Preserved; the advertising interval is the keep-alive               |
| Never emit 0.0; absence means zero             | Preserved — the record is omitted                                   |
| Highest intensity wins per tag                 | Preserved, unchanged, still receiver-side                           |
| Devices emit several tags; consumers filter    | Preserved and improved — all tags in one atomic packet              |
| Several networks coexisting in one space       | Preserved via the 16-bit Session ID                                 |
| Each toy keeps its regular API                 | Preserved — a second advertising set on BLE 5, time-sliced on 4.2   |
| Not a pattern generator                        | Unchanged; ~150 ms latency suits a ≤5 Hz behavioural signal         |
| Simplicity of setup                            | Improved — four steps become three                                  |

Two things are strictly better: setup loses a step and a single point of failure, and battery
devices become practical. One thing is strictly worse: the medium is open, so allow-listing moves
from optional to expected. One thing is bounded differently: range becomes a room rather than a
building, which is a limitation for large spaces and a security property everywhere else.

## Open Questions

- **Session code ergonomics.** A hashed passphrase is friendlier than four hex digits, but two
  users choosing the same word land in the same session. Is that a bug or a feature?
- **Should the decay time be per-emitter?** A one-byte field in 100 ms units would let a slow
  sensing toy stretch its 1.5 s. The original spec fixes it; making it negotiable adds a knob
  and a way to get it wrong.
- **Tag namespace growth.** Codes `0x06`–`0xFE` are open. Adding well-known codes is cheap, but
  every addition is a compatibility question for older consumers, which should ignore unknown
  codes rather than treat them as custom.
- **Bridging between sessions.** Nothing prevents a device from bridging two session IDs, which
  could be either a useful way to federate rooms or a good way to surprise somebody.
