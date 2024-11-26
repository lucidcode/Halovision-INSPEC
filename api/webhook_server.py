import requests
import json

inspec_ip = '192.168.20.24'
inspec_port = 5000
inspec_url = f'http://{inspec_ip}:{inspec_port}'

webhook_server = '127.0.0.1'
webhook_port = 5000
webhook_url = f'http://{webhook_server}:{webhook_port}'

while True:

    try:
        print(f'Wating for INSPEC on {inspec_ip}:{inspec_port}...')
        response = requests.get(inspec_url, stream=True, timeout=5)
        print("Connected", inspec_url)
        
        for stream in response.iter_lines(3, decode_unicode=None):
            data = stream.decode()
            if data == '':
                continue

            print(data)
            parts = data.split(':')            
            endpoint = f'{webhook_url}/variance'            
            message = { 'variance': 0 }

            if len(parts) == 1:                
                message = { 'variance': data }
            else:
                endpoint = f'{webhook_url}/{parts[0]}'
                message = { parts[0]: parts[1] }

            try:
                json_message = json.dumps(message)
                requests.post(endpoint, data=json_message, headers={'Content-Type': 'application/json'})
            except Exception as re:
                print(str(re))

    except Exception as e:
        print(str(e))
