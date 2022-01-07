while :
do
cd /var/www/html/LED-Cube/WEB/animations
ls |sort -R |tail -$N |while read file; do
    # Something involving $file, or you can leave
    # off the while to just get the filenames
    sudo pkill -SIGINT execLEDCube
    echo $file
    sudo /var/www/html/LED-Cube/cpp/execLEDCube $file &
    sleep 10
done
done
