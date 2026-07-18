# Dokumentasi API Config System

Dokumentasi ini menjelaskan cara menggunakan API Config System di dalam codebase Noxe Editor.

---

## 🛠️ API Reference (`src/config.h`)

Untuk menggunakan fungsi-fungsi ini di file C baru, pastikan Anda menyertakan header berikut:
```c
#include "config.h"
```

### 1. Inisialisasi & Loading

#### `config_init`
Menginisialisasi konfigurasi awal dengan nilai default (`tabsize` = 4, `mouse` = true).
```c
void config_init(void);
```

#### `config_load`
Membaca konfigurasi dari file lokal `./noxe.json`, file global `~/.noxerc` (jika ada), serta argumen baris perintah (CLI).
```c
void config_load(int argc, char* argv[]);
```
* **Contoh penggunaan di CLI**: `./bin/noxe --tabsize 8 --mouse false`

---

### 2. Membaca Konfigurasi (Getters)

Untuk mengambil nilai konfigurasi secara dinamis, gunakan fungsi getter berikut sesuai tipe data yang dibutuhkan. Jika key tidak ditemukan atau tipe data tidak cocok, fungsi akan mengembalikan nilai default yang Anda tentukan.

#### `config_get_number`
Mengambil nilai konfigurasi bertipe numeric (double).
```c
double config_get_number(const char* key, double default_val);
```
* **Contoh**: `int tab_size = (int)config_get_number("tabsize", 4);`

#### `config_get_bool`
Mengambil nilai konfigurasi bertipe boolean (int 0 atau 1).
```c
int config_get_bool(const char* key, int default_val);
```
* **Contoh**: `int use_mouse = config_get_bool("mouse", 1);`

#### `config_get_string`
Mengambil nilai konfigurasi bertipe string.
```c
const char* config_get_string(const char* key, const char* default_val);
```
* **Contoh**: `const char* theme = config_get_string("theme", "default");`

---

### 3. Mengubah Konfigurasi (Setters)

Mengubah nilai konfigurasi di memori sekaligus menyimpannya (menserialisasikannya) ke file konfigurasi aktif secara otomatis.

#### `config_set_number`
```c
void config_set_number(const char* key, double val);
```

#### `config_set_bool`
```c
void config_set_bool(const char* key, int val);
```

#### `config_set_string`
```c
void config_set_string(const char* key, const char* val);
```

---

### 4. Live Reloading & Validation

#### `config_watch`
Mengecek perubahan modifikasi waktu (`mtime`) pada file config secara real-time. Jika file berubah di disk, fungsi ini otomatis me-reload nilainya ke memori dan menampilkan notifikasi di status bar. Panggil ini di main loop program.
```c
void config_watch(struct global* global);
```

#### `config_validate`
Melakukan pengecekan validasi tipe data dari input string raw sebelum dimasukkan ke konfigurasi. Mengembalikan `1` jika valid, atau `0` jika ada kesalahan schema.
```c
int config_validate(const char* key, const char* raw_val, SchemaError* err_out);
```
