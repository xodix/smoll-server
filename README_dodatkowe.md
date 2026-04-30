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

# Sposób uzyskiwania informacji o platformach

```bash
sudo docker buildx imagetools inspect pan1jan1/smol-server:v2.0
```

```
Name:      docker.io/pan1jan1/smol-server:v2.0
MediaType: application/vnd.oci.image.index.v1+json
Digest:    sha256:1029b3b80bf6d3a22337e9497f01b00d50f7deee790be7c687aef75228ced8bb

Manifests:
  Name:        docker.io/pan1jan1/smol-server:v2.0@sha256:836d42793add2ceaab03c8d7563f88ab35c3602becda8e1c3a3ae752ceda5f9c
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    linux/amd64

  Name:        docker.io/pan1jan1/smol-server:v2.0@sha256:29dc80635b2502d2c71a38266d6a34434499bc441786c3e1fd571f9419f25d58
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    linux/arm64

  Name:        docker.io/pan1jan1/smol-server:v2.0@sha256:c2cb5ff855c3f025802ed4ab43a9d0e0307ee330b89e386e3a7decd33a88c6dd
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    unknown/unknown
  Annotations:
    vnd.docker.reference.digest: sha256:836d42793add2ceaab03c8d7563f88ab35c3602becda8e1c3a3ae752ceda5f9c
    vnd.docker.reference.type:   attestation-manifest

  Name:        docker.io/pan1jan1/smol-server:v2.0@sha256:c768ed2883fd6e03516db7837c9ad929d742eb65e3ff07356ee4f496494c3318
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    unknown/unknown
  Annotations:
    vnd.docker.reference.type:   attestation-manifest
    vnd.docker.reference.digest: sha256:29dc80635b2502d2c71a38266d6a34434499bc441786c3e1fd571f9419f25d58
```
