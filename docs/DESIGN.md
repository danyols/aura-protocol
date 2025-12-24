# AURA (Agentic UDP Reliability & Adaptation)

## Purpose

- Embedded C++ development
- Network protocol design (UDP + custom reliability)
- AI/Agentic workflows for autonomous optimization
- Systems thinking

---

### ARQ (Automatic Repeat Request)
A strategy for reliability: if don't get an ACK, send again.

### Why UDP Instead of TCP?
- **TCP:** Reliable but heavy, causes "head-of-line blocking" (one lost packet freezes everything)
- **UDP:** Lightweight, low latency, but no built-in reliability
- **My approach:** UDP + custom ARQ = we control exactly what gets retried vs dropped

