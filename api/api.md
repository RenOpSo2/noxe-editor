# Panduan Konfigurasi Noxe Editor (User Guide)

Panduan ini menjelaskan cara melakukan konfigurasi dan menggunakan Config System pada Noxe Editor untuk menyesuaikan kenyamanan mengetik Anda.

---

## 📂 Lokasi File Konfigurasi

Noxe Editor mendukung pembacaan konfigurasi secara otomatis dari dua lokasi berikut (diutamakan file lokal terlebih dahulu):

1. **Lokal (Project-level)**: `./noxe.json` (file `noxe.json` di direktori tempat Anda menjalankan editor).
2. **Global (User-level)**: `~/.noxerc` (file `.noxerc` di direktori home pengguna Anda).

---

## ⚙️ Opsi Konfigurasi yang Didukung

Saat ini, Noxe Editor mendukung beberapa opsi konfigurasi berikut:

| Nama Kunci | Tipe Data | Nilai Default | Deskripsi |
| :--- | :--- | :--- | :--- |
| `tabsize` | Angka (Number) | `4` | Menentukan lebar visual karakter tab (`\t`) ketika ditampilkan di layar editor (antara 1 s.d 16). |
| `mouse` | Boolean | `true` | Mengaktifkan/menonaktifkan fungsionalitas pendukung mouse (nilai yang valid: `true`, `false`, `1`, `0`). |

---

## 📝 Format File JSON

Format penulisan di dalam file `./noxe.json` atau `~/.noxerc` wajib menggunakan standar JSON yang valid. 

**Contoh isi file konfigurasi:**
```json
{
  "tabsize": 4,
  "mouse": true
}
```

---

## 🚀 Penggunaan via CLI (Command Line Interface)

Anda juga bisa menimpa (override) pengaturan konfigurasi secara langsung melalui argumen baris perintah saat membuka Noxe Editor. Argumen CLI ini memiliki prioritas tertinggi dibandingkan file JSON.

### Contoh Perintah:

* **Membuka file dengan ukuran tab = 8:**
  ```bash
  ./bin/noxe --tabsize 8 nama_file.txt
  ```

* **Mematikan fungsi mouse dan set tab = 2:**
  ```bash
  ./bin/noxe --tabsize 2 --mouse false nama_file.txt
  ```

---

## 🔄 Pembaruan Otomatis (Live Watch / Auto-Reload)

Noxe Editor memiliki fitur pemantauan file konfigurasi secara langsung. 

Jika Anda mengubah pengaturan di dalam file `./noxe.json` atau `~/.noxerc` menggunakan aplikasi lain selagi Noxe Editor sedang terbuka, Noxe Editor akan:
1. Mendeteksi perubahan berkas secara otomatis.
2. Memuat ulang (re-parse) pengaturan baru tanpa perlu menutup editor.
3. Menampilkan pesan `"Configuration reloaded automatically."` pada status bar bagian bawah layar.
