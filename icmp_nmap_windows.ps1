<#
Usage (run as Admin):
.\icmp_nmap_windows.ps1 -Target 192.168.0.123 -Interface "Ethernet" -DataLength 1400 -MaxRate 2000 -Duration 10

Requires: nmap in PATH, tshark (from Wireshark) in PATH.
#>

param(
  [Parameter(Mandatory=$true)][string]$Target,
  [Parameter(Mandatory=$true)][string]$Interface,
  [int]$DataLength = 1400,
  [int]$MaxRate = 2000,
  [int]$Duration = 10
)

# Locations (assume in PATH)
$nmap = "nmap"
$tshark = "tshark"

# check tools
if (-not (Get-Command $nmap -ErrorAction SilentlyContinue)) {
  Write-Error "nmap не найден в PATH. Установи Nmap и добавь в PATH."
  exit 1
}
if (-not (Get-Command $tshark -ErrorAction SilentlyContinue)) {
  Write-Error "tshark не найден в PATH. Установи Wireshark (tshark) и добавь в PATH."
  exit 1
}

$pcap = Join-Path $env:TEMP ("icmp_nmap_" + (Get-Random) + ".pcap")
Write-Host "Capture file: $pcap"

# start tshark capture for fixed duration (filter ICMP to reduce file size)
$tsharkArgs = "-i `"$Interface`" -f `"icmp and host $Target`" -w `"$pcap`" -a duration:$Duration"
Write-Host "Starting tshark (duration ${Duration}s)..."
$tsProc = Start-Process -FilePath $tshark -ArgumentList $tsharkArgs -WindowStyle Hidden -PassThru

# small delay to ensure capture started
Start-Sleep -Milliseconds 700

# start nmap to generate ICMP packets
$nmapArgs = "-sn -PE --data-length $DataLength --max-rate $MaxRate -T4 $Target"
Write-Host "Running: nmap $nmapArgs"
$nmProc = Start-Process -FilePath $nmap -ArgumentList $nmapArgs -NoNewWindow -Wait -PassThru

# wait until tshark finishes (it auto-stops by duration)
$tsProc.WaitForExit()

# count ICMP echo-request (type 8) to the target
# use tshark display filter: icmp.type == 8 and ip.dst == $Target
Write-Host "Counting ICMP echo-request in pcap..."
$tsharkCountArgs = "-r `"$pcap`" -Y `"icmp.type==8 and ip.dst==${Target}`" -T fields -e frame.number"
$tsharkOut = & $tshark $tsharkCountArgs 2>$null
$reqCount = 0
if ($tsharkOut) { $reqCount = ($tsharkOut | Measure-Object -Line).Lines }

$ipHdr = 20; $icmpHdr = 8
$bytesPerPacket = $DataLength + $ipHdr + $icmpHdr
$bitsPerPacket = $bytesPerPacket * 8
$totalBits = $reqCount * $bitsPerPacket
$mbps = 0
if ($Duration -gt 0) {
  $mbps = [math]::Round($totalBits / ($Duration * 1e6), 3)
}

Write-Host ""
Write-Host "Target: $Target"
Write-Host "Interface: $Interface"
Write-Host "Duration: ${Duration}s"
Write-Host "Packets captured (ICMP echo-request): $reqCount"
Write-Host "Payload (--data-length): $DataLength bytes"
Write-Host "Bytes/packet (incl. IP/ICMP hdr): $bytesPerPacket"
Write-Host "Estimated ICMP throughput: $mbps Mbit/s"
Write-Host ""
Write-Host "Pcap saved to: $pcap"
