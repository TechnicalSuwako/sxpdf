#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <poppler/glib/poppler.h>
#include <X11/keysym.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

PopplerDocument *doc = NULL; 
int curpage = 0;
int totpage = 0;
//char sq[256] = {0}; // TODO: 検索

void cairo_surface_clear(cairo_surface_t *surface) {
  cairo_t *cr = cairo_create(surface);
  cairo_set_source_rgba(cr, 0, 0, 0, 0);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
  cairo_destroy(cr);
}

// TODO: 検索
/*void search(PopplerDocument *doc, const char *keyword, cairo_t *cr) {
  int n_page = poppler_document_get_n_pages(doc);

  for (int i = 0; i < n_page; i++) {
    PopplerPage *page = poppler_document_get_page(doc, i);
    if (!page) continue;

    GList *matches = poppler_page_find_text(page, keyword);
    for (GList *l = matches; l; l = l->next) {
      PopplerRectangle *match = (PopplerRectangle *)l->data;

      cairo_set_source_rgba(cr, 1, 1, 0, 0.5);
      cairo_rectangle(cr, match->x1, match->y1, match->x2 - match->x1, match->y2 - match->y1);
      cairo_fill(cr);

      poppler_rectangle_free(match);
    }
    g_list_free(matches);
    g_object_unref(page);
  }
}*/

void open_pdf(const char* filename) {
  GError *error = NULL;
  char file_uri[512];
  snprintf(file_uri, sizeof(file_uri), "file://%s", filename);
  doc = poppler_document_new_from_file(file_uri, NULL, &error);
  if (doc == NULL) {
    printf("PDFファイルを読み込みに失敗しました： %s\n", error->message);
    return;
  }

  curpage = 0;
  totpage = poppler_document_get_n_pages(doc);

  PopplerPage *page = poppler_document_get_page(doc, curpage);
  if (page == NULL) {
    printf("ページを読み込みに失敗しました。\n");
    return;
  }

  double pag_w, pag_h;
  poppler_page_get_size(page, &pag_w, &pag_h);

  Display *d = XOpenDisplay(NULL);
  Window w = XCreateSimpleWindow(d, RootWindow(d, 0), 0, 0, pag_w, pag_h, 1, 0, 0);
  XSelectInput(d, w, ExposureMask | KeyPressMask | StructureNotifyMask);
  XMapWindow(d, w);

  cairo_surface_t *xlib_surface = cairo_xlib_surface_create(d, w, DefaultVisual(d, 0), pag_w, pag_h);
  cairo_t *xlib_cr = cairo_create(xlib_surface);

  double scale = 1.2;
  double xpos = 0.0;
  double ypos = 0.0;
  // TODO: 検索
  //bool isfind = false;
  //int sqlen = 0;

  while (1) {
    XEvent e;
    XNextEvent(d, &e);

    if (e.type == KeyPress) {
      XWindowAttributes attrs;
      XGetWindowAttributes(d, w, &attrs);

      KeySym keysym = XLookupKeysym(&e.xkey, 0);
      double pscale = scale;

      if (keysym == XK_q) {
        break;
      } else if (keysym == XK_plus || keysym == XK_semicolon) {
        scale += 0.1;
        if (scale > 5.0) scale = 5.0;
        xpos -= (pag_w * scale - pag_w * pscale) / 2.0;
        ypos -= (pag_h * scale - pag_h * pscale) / 2.0;
      } else if (keysym == XK_minus || keysym == XK_equal) {
        scale -= 0.1;
        if (scale < 0.1) scale = 0.1;
        xpos += (pag_w * pscale - pag_w * scale) / 2.0;
        ypos += (pag_h * pscale - pag_h * scale) / 2.0;
      } else if (keysym == XK_h) {
        xpos -= 10.0;
        if (xpos < 0) xpos = 0;
      } else if (keysym == XK_l) {
        xpos += 10.0;
        if (xpos > pag_w * scale - attrs.width) xpos = pag_w * scale - attrs.width;
      } else if (keysym == XK_k) {
        ypos -= 10.0;
        if (ypos < 0) ypos = 0;
      } else if (keysym == XK_j) {
        ypos += 10.0;
        if (ypos > pag_h * scale - attrs.height) ypos = pag_h * scale - attrs.height;
      } else if (keysym == XK_comma || keysym == XK_less) {
        if (curpage > 0) {
          curpage--;
          if (curpage < 0) curpage = 0;
          xpos = 0.0;
          ypos = 0.0;
          page = poppler_document_get_page(doc, curpage);
        }
      } else if (keysym == XK_period || keysym == XK_greater) {
        if (curpage < totpage - 1) {
          curpage++;
          if (curpage >= totpage) curpage = totpage - 1;
          xpos = 0.0;
          ypos = 0.0;
          page = poppler_document_get_page(doc, curpage);
        }
      // TODO: 検索
      /*} else if (keysym == XK_slash) {
        if (!isfind) {
          isfind = true;
          memset(sq, 0, sizeof(sq));
          sqlen = 0;
        }
        continue;
      } else if (keysym == XK_Escape) {
        if (isfind) {
          isfind = false;
          memset(sq, 0, sizeof(sq));
          sqlen = 0;
          continue;
        }*/
      }

      // TODO: 検索
      /*if (isfind) {
        char str[25];
        int len = XLookupString(&e.xkey, str, sizeof(str), NULL, NULL);
        if (len > 0 && sqlen + len < sizeof(sq) - 1) {
          strncpy(sq + sqlen, str, len);
          sqlen += len;
          sq[sqlen] = '\0';
          search(doc, sq, xlib_cr);
        } else if (keysym == XK_BackSpace && sqlen > 0) {
          sq[--sqlen] = '\0';
          search(doc, sq, xlib_cr);
        }
        continue;
      }*/

      XClearWindow(d, w);
      XExposeEvent ev = {0};
      ev.type = Expose;
      ev.window = w;
      XSendEvent(d, w, False, ExposureMask, (XEvent*)&ev);
      XFlush(d);
    }

    if (e.type == ConfigureNotify) {
      pag_w = e.xconfigure.width;
      pag_h = e.xconfigure.height;
      cairo_xlib_surface_set_size(xlib_surface, pag_w, pag_h);
    }

    if (e.type == Expose) {
      XWindowAttributes attrs;
      XGetWindowAttributes(d, w, &attrs);

      // 表示を削除
      cairo_identity_matrix(xlib_cr);
      cairo_set_source_rgb(xlib_cr, 1, 1, 1);
      cairo_paint(xlib_cr);

      // 表示を作成
      cairo_translate(xlib_cr, xpos, ypos);
      cairo_scale(xlib_cr, scale, scale);

      // TODO: ページ番号の表示
      /*cairo_identity_matrix(xlib_cr);
      char pinf[100];
      snprintf(pinf, sizeof(pinf), "%d/%dページ", curpage + 1, totpage);
      cairo_text_extents_t te;
      cairo_select_font_face(xlib_cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size(xlib_cr, 14);
      cairo_text_extents(xlib_cr, pinf, &te);
      cairo_move_to(xlib_cr, 10, 20);
      cairo_set_source_rgb(xlib_cr, 0, 0, 0);
      cairo_show_text(xlib_cr, pinf);*/

      cairo_status_t status = cairo_status(xlib_cr);
      if (status != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Cairoエラー： %s\n", cairo_status_to_string(status));
      }

      // TODO: 検索
      //search(doc, sq, xlib_cr);
      poppler_page_render(page, xlib_cr);
      cairo_surface_flush(xlib_surface);
    }
  }

  cairo_destroy(xlib_cr);
  cairo_surface_destroy(xlib_surface);
  g_object_unref(page);
  g_object_unref(doc);
  XCloseDisplay(d);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    g_print("使用方法： %s <PDFファイル>\n", argv[0]);
    return 1;
  }

  open_pdf(argv[1]);

  return 0;
}
