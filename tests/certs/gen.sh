#!/usr/bin/env bash
# Regenerates the certificates used by the host TLS unit tests (test_tls.cpp).
# UNIT-TEST ONLY: unrelated to the CA the firmware trusts; keys are public.
set -euo pipefail
export MSYS_NO_PATHCONV=1
cd "$(dirname "$0")"

ec_key() { openssl ecparam -name prime256v1 -genkey -noout -out "$1"; }

ec_key ca.key
openssl req -x509 -new -key ca.key -sha256 -days 7300 -out ca.crt \
  -subj "/O=MCORE/CN=MCORE UNIT TEST CA" \
  -addext "basicConstraints=critical,CA:TRUE" -addext "keyUsage=critical,keyCertSign,cRLSign"

ec_key other_ca.key
openssl req -x509 -new -key other_ca.key -sha256 -days 7300 -out other_ca.crt \
  -subj "/O=Elsewhere/CN=UNTRUSTED CA" \
  -addext "basicConstraints=critical,CA:TRUE" -addext "keyUsage=critical,keyCertSign,cRLSign"

sign() {   # name key ca cakey subject ext
  openssl req -new -key "$2" -subj "$5" -out "$1.csr"
  printf '%s\n' "$6" > "$1.ext"
  openssl x509 -req -in "$1.csr" -CA "$3" -CAkey "$4" -CAcreateserial -days 7300 \
    -sha256 -extfile "$1.ext" -out "$1.crt" 2>/dev/null
  rm -f "$1.csr" "$1.ext"
}
srv_ext="basicConstraints=CA:FALSE
keyUsage=critical,digitalSignature,keyEncipherment
extendedKeyUsage=serverAuth
subjectAltName=DNS:broker.test,IP:10.0.0.1"

ec_key server.key
sign server server.key ca.crt ca.key "/CN=broker.test" "$srv_ext"
openssl genrsa -out server_rsa.key 2048 2>/dev/null
sign server_rsa server_rsa.key ca.crt ca.key "/CN=broker.test" "$srv_ext"
ec_key rogue.key
sign rogue rogue.key other_ca.crt other_ca.key "/CN=broker.test" "$srv_ext"
ec_key client.key
sign client client.key ca.crt ca.key "/CN=agv-unit-test" "basicConstraints=CA:FALSE
keyUsage=critical,digitalSignature
extendedKeyUsage=clientAuth"

rm -f ./*.srl other_ca.key
echo "unit-test certificates regenerated"
