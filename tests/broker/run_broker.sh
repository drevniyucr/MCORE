#!/usr/bin/env bash
# Runs a local mosquitto broker for MCORE tests (needs gen_pki.sh first):
#   8883  MQTT over TLS 1.2, user/password required, client certs optional
#   1883  plain MQTT on 127.0.0.1 only, anonymous (for mosquitto_sub/pub)
# Test account: agv001 / mcore-test
set -euo pipefail
export MSYS_NO_PATHCONV=1

here="$(cd "$(dirname "$0")" && pwd)"
pki="$here/pki"
[ -f "$pki/server.crt" ] || { echo "run gen_pki.sh first" >&2; exit 1; }

winpath() { if command -v cygpath >/dev/null; then cygpath -m "$1"; else echo "$1"; fi; }

if [ ! -f "$pki/passwd" ]; then
  : > "$pki/passwd"
  mosquitto_passwd -b "$(winpath "$pki/passwd")" agv001 mcore-test
fi

conf="$pki/mosquitto.conf"
cat > "$conf" <<EOF
per_listener_settings true

listener 8883
cafile $(winpath "$pki/ca.crt")
certfile $(winpath "$pki/server.crt")
keyfile $(winpath "$pki/server.key")
tls_version tlsv1.2
require_certificate false
allow_anonymous false
password_file $(winpath "$pki/passwd")

listener 1883 127.0.0.1
allow_anonymous true
EOF

exec mosquitto -c "$(winpath "$conf")" -v
