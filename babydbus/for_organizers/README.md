# babydbus (Pwnable)

문제 지문은 DESCRIPTION.txt에 있습니다. IP는 서버 IP로 바꿔주세요.

dbus 프로토콜로 리스닝하는 서비스를 연결하는 법, 그리고 메서드를 호출하는 법을
묻는 문제입니다. 처음 dbus를 접하는 사람들을 위해 서버에 규격에 맞는 요청만 보내면
플래그를 주도록 하였습니다.

## 설치하는 법

일단 도커를 설치해주세요.

```sh
cd dist
# sudo가 필요할 수 있습니다. 안되면 모든 도커 커맨드 앞에 sudo를 붙여주세요 (sudo docker)
docker build -t babydbus .
```

## 서버를 구동하는 법

터미널에서 아래의 명령어를 실행해주시면 됩니다. 편의에 따라 tmux 안에서 구동해주세요.

```sh
# 마찬가지로 sudo가 필요할 수 있습니다.
docker run -p2222:22 -it babydbus
```

## 서버에 연결하는 법

```sh
# 비밀번호는 user 입니다
ssh user@127.0.0.1 -p2222
```

## 익스플로잇, 플래그

ssh 연결 후 src/exploit.sh를 복사 붙여넣기 해주세요.

```
KOREA{a666ff28299b901a78bad5cc7cf1ba77}
```