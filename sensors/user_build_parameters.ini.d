[env]
build_flags =
    -DDEBUG
    -DINITIAL_STARTUP_DELAY=5000
    
    '-DDB_USER="postgres"'
    '-DDB_PASSWORD="password123"'
    '-DDB_PASSWORD="your_db"'
    '-DDB_ADDRESS=192,168,1,50'
    '-DDB_PORT=5432'

    '-DWLAN_IPV4_ADDRESS=192,168,1,200'
    '-DWLAN_IPV4_GATEWAY=192,168,1,1'
    '-DWLAN_IPV4_SUBNET_MASK=255,255,255,0'
    '-DWLAN_IPV4_DNS1=8,8,8,8'
    '-DWLAN_IPV4_DNS2=8,8,9,9'

    '-DWLAN_SSID="ssid"'
    '-DWLAN_WPA2KEY="password"'
