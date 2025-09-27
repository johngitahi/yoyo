# yoyo 🔑

**yoyo* is a lightweight local password manager app written in C.
It uses [libsodium](https://doc.libsodium.org/) for encryption and [Jansson](https://digip.org/jansson/) for JSON management.

Passwords are encrypted with a master key derived from a master password which the user sets, and the vault is stored at `$YOYO_VAULT`.


## Features
- 🔐 Strong encryption with [libsodium](https://doc.libsodium.org/).
- 📂 Vault stored locally as an encrypted JSON file (`$YOYO_VAULT`).
- Simple CLI commands:
  - `yoyo init` - initialize a new vault.
  - `yoyo add <service> <uid> <password> - add an entry
  - (others not implemented yet. check them in .idea)
  

### Installation

### Prerequisites
- A C compiler (`clang` or `gcc`).
- `libsodium` and `jansson` (install with your package manager/ from their websites)

### Build
    clang -o yoyo yoyo.c common.c -ljansson -lsodium

---

## Usage

### Initialize a vault
    yoyo init

- You’ll be asked to enter and confirm a master password.
- Creates ~/.yoyo (encrypted vault).
 **note**: currently yoyo stores the vault it in /home/gth/.yoyo so you'd need to create a gth home dir to test it now, or you can just create a PR to make $YOYO_VAULT a thing.

### Add a password
    yoyo add <service> <username/email> <password>

Example:
    yoyo add github alice@example.com s3cr3tpass

### Help
    yoyo add -h


## Security Notes

- Your vault never leaves your machine.
- A new salt and nonce are used for each encryption.
- The master password is never stored, only used to derive a key.
- Sensitive memory is wiped using sodium_memzero.

---

## Roadmap

- [ ] `yoyo list` – show saved services.
- [ ] `yoyo ret <service>` – retrieve a password securely.
- [ ] `yoyo rm <service>` – remove an entry.
- [ ] `yoyo gen` – generate a secure password
- [ ] Configurable vault location
- [ ] Better error messages and UX polish.
- [ ] A Chrome extension to connect to yoyo.