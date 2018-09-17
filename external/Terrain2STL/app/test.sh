

if curl -f https://dds.cr.usgs.gov/srtm/version2_1/SRTM3/Eurasia/N54W002.hgt.zip >/dev/null 2>&1; then
echo "FOUND"
else
echo "NOT FOUND"
fi
