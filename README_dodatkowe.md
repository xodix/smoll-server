# Tworzenie buildera

```bash
docker buildx create --name smol-builder --driver docker-container --use --bootstrap
```

# Budowanie obrazu

```bash
eval $(ssh-agent -s) # lub -c dla fish shell
# set -Ux SSH_AUTH_SOCK $SSH_AUTH_SOCK # fish
# set -Ux SSH_AGENT_PID $SSH_AGENT_PID # fish
ssh-add ~/.ssh/github
sudo docker buildx build -f Dockerfile_dodatkowe \
			--push --platform linux/amd64,linux/arm64 \
			-t pan1jan1/smol-server:v2.0 \
			--ssh github=$HOME/.ssh/github \
			--cache-to type=registry,ref=pan1jan1/smol-server:cache,mode=max \
			--cache-from type=registry,ref=pan1jan1/smol-server:cache .
```

# Uruchomienie serwera

```bash
docker run -d -p 3000:3000 --name smol-server pan1jan1/smol-server:v2.0
```

# Sposób uzyskiwania informacji z logów

```bash
docker logs smol-server
```
