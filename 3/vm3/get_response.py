import requests

url = "http://pandalism.net:8080"

try:
    response = requests.get(url, timeout=5)
    print(f"Status Code: {response.status_code}")
    print(response.text)
except requests.exceptions.RequestException as e:
    print(f"Error: {e}")
