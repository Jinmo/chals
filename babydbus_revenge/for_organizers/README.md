# babydbus_revenge (Pwnable)

문제 지문은 DESCRIPTION.txt에 있습니다. IP는 서버 IP로 바꿔주세요.

babydbus에서 포너블 요소를 추가했습니다. `syslog` 함수는 포맷 스트링을
전달받기 때문에 포맷을 넣어야 하는데, 유저가 해당 포맷 스트링을 바꿀
수 있어서 나는 Format String Bug 문제입니다.

## 설치하는 법

babydbus와 같습니다. 일단 도커를 설치해주세요. 컨테이너 이름만 바꾸겠습니다.

```sh
cd dist
# sudo가 필요할 수 있습니다. 안되면 모든 도커 커맨드 앞에 sudo를 붙여주세요 (sudo docker)
docker build -t babydbus_revenge .
```

## 서버를 구동하는 법

터미널에서 아래의 명령어를 실행해주시면 됩니다. 편의에 따라 tmux 안에서 구동해주세요.

```sh
# 마찬가지로 sudo가 필요할 수 있습니다.
docker run -p2223:22 -it babydbus_revenge
```

## 서버에 연결하는 법

```sh
# 비밀번호는 user 입니다
ssh user@127.0.0.1 -p2223
```

## 익스플로잇, 플래그

SSH 연결 후 파이썬을 실행하시고 src/exploit.py의 내용을 붙여넣어주세요.
익스플로잇에서는 FSB를 이용하여 전역 변수로 선언된, 5초마다 실행되는 커맨드를 덮어썼습니다.

```
KOREA{3b2872559a194622f0090f8f02cffb3f}
```