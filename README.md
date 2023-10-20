# sxpdf

シンプルなPDFリーダー

# 使い方
q = 終了\
hjkl = 移動\
-;=+ = ズーム\
,.<> = ページを変更

# 従属ソフト
* poppler
* xorg

## CRUX
doas prt-get depinst poppler

## Artix
doas pacman -S poppler

## OpenBSD
doas pkg_add poppler

## FreeBSD
doas pkg install poppler

## PostmarketOS
doas apk add poppler

## Devuan
doas apt install libpoppler-dev
