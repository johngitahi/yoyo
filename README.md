# yoyo 🔑

**yoyo** is a lightweight local password manager app written in C.  
It uses [libsodium](https://doc.libsodium.org/) for encryption and [Jansson](https://digip.org/jansson/) for JSON management.

Passwords are encrypted with a master key derived from a master password which the user sets, and the vault is stored at `$YOYO_VAULT`.

---

## Features
- Strong encryption with [libsodium](https://doc.libsodium.org/).
- Vault stored locally as an encrypted JSON file (`$YOYO_VAULT`).
- Simple CLI commands:
  - `yoyo init` – Initialize a new vault and set a master password.
  - `yoyo add <service> <uid> <password>` – Add a new password to the vault.
  - `yoyo get <service>` – Get a saved password for a specific service.
  - `yoyo list` – List all the passwords in the Yoyo vault.
  - `yoyo status` – Show status of yoyo on your local machine.
  - `yoyo gen` – Generate a strong password (shown on stdout).

---

## Installation

### Prerequisites
- `clang` C compiler.
- [libsodium](https://doc.libsodium.org/) and [jansson](https://jansson.readthedocs.io/en/latest/)  
  *(install with your package manager or refer to their websites for instructions).*

### Build
```bash
clang -o yoyo -lsodium -ljansson yoyo.c vault.c clipboard.c gen.c
```

### Archlinux
Aur maintained by me.
```bash
yay -Sy yoyo
```

---

## Usage

### Initialize a vault
```bash
yoyo init
```

- You’ll be asked to enter and confirm a master password.
- Creates `~/.yoyo` (encrypted vault).

### Add a password
```bash
yoyo add <service> <username/email> <password>
```

Example:
```bash
yoyo add github alice@example.com s3cr3tpass
```

### Help
```bash
yoyo add -h
```

### Status
```bash
yoyo status
```

- Shows whether the vault is initialized.
- Displays vault path.

### Get a password
```bash
yoyo get <service>
```

- Copies the password for `<service>` into the clipboard (for 1 minute).
- Example:
```bash
yoyo get github
```

### List stored services
```bash
yoyo list
```

- Prints all stored service names (but not passwords).
- Example:
```bash
yoyo list
```

---

## Security Notes

- Your vault never leaves your machine.
- A new nonce is used for each encryption.
- The master password is never stored, only used to derive a key.
- Sensitive memory is wiped using `sodium_memzero`.

**You can use the `devtest` tool to see what the yoyo vault looks like.** 

---

## Roadmap

- [x] `yoyo list` – show saved services.
- [x] `yoyo get <service>` – retrieve a password securely.
- [ ] `yoyo rm <service>` – remove an entry.
- [x] `yoyo gen` – generate a secure password.
- [ ] Better error messages and UX polish.
- [ ] A Chrome extension to connect to yoyo.
