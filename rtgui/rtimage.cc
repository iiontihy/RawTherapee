/*
 *  This file is part of RawTherapee.
 *
 *  Copyright (c) 2004-2010 Gabor Horvath <hgabor@rawtherapee.com>
 *  Copyright (c) 2011 Jean-Christophe FRISCH <natureh@free.fr>
 *
 *  RawTherapee is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RawTherapee is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RawTherapee.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <map>
#include <set>

#include "rtimage.h"

#include "options.h"

#include "../rtengine/icons.h"

namespace
{

using PixbufCache = std::map<std::string, Glib::RefPtr<Gdk::Pixbuf>>;

PixbufCache pixbuf_cache;

}

RTImage::RTImage(const Glib::ustring& file_name, const Glib::ustring& rtl_file_name) :
    Gtk::Image()
{
    changeImage(
        !rtl_file_name.empty() && get_direction() == Gtk::TEXT_DIR_RTL
            ? rtl_file_name
            : file_name
    );
}

void RTImage::changeImage(const Glib::ustring& image_name)
{
    clear();

    PixbufCache::const_iterator entry = pixbuf_cache.find(image_name);

    if (entry == pixbuf_cache.end()) {
        try {
            const Glib::ustring imagePath = rtengine::findIconAbsolutePath(image_name);
            const Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(imagePath);

            entry = pixbuf_cache.emplace(image_name, pixbuf).first;
        } catch (const Glib::Exception& exception) {
        }
    }

    if (entry != pixbuf_cache.end()) {
        set(entry->second);
    }
}

void RTImage::updateImages()
{
    std::set<Glib::ustring> invalid_entries;

    for (auto& entry : pixbuf_cache) {
        try {
            const Glib::ustring image_path = rtengine::findIconAbsolutePath(entry.first);
            entry.second = Gdk::Pixbuf::create_from_file(image_path);
        }  catch (const Glib::Exception& exception) {
            invalid_entries.insert(entry.first);
        }
    }

    for (const auto& invalid_entry : invalid_entries) {
        pixbuf_cache.erase(invalid_entry);
    }
}

void RTImage::cleanup()
{
    pixbuf_cache.clear();
}

Glib::RefPtr<Gdk::Pixbuf> RTImage::createFromFile(const Glib::ustring& file_name)
{
    Glib::RefPtr<Gdk::Pixbuf> pixbuf;

    try {
        const Glib::ustring file_path = rtengine::findIconAbsolutePath(file_name);

        if (!file_path.empty()) {
            pixbuf = Gdk::Pixbuf::create_from_file(file_path);
        }
    } catch (const Glib::Exception& exception) {
        if (options.rtSettings.verbose) {
            std::cerr << "Failed to load image \"" << file_name << "\": " << exception.what() << std::endl;
        }

    }

    return pixbuf;
}

Cairo::RefPtr<Cairo::ImageSurface> RTImage::createFromPng(const Glib::ustring& file_name)
{
    Cairo::RefPtr<Cairo::ImageSurface> surface;

    try {
        const Glib::ustring file_path = rtengine::findIconAbsolutePath(file_name);

        if (!file_path.empty()) {
            surface = Cairo::ImageSurface::create_from_png(Glib::locale_from_utf8(file_path));
        }

    } catch (const Glib::Exception& exception) {
        if (options.rtSettings.verbose) {
            std::cerr << "Failed to load PNG \"" << file_name << "\": " << exception.what() << std::endl;
        }
    }

    return surface;
}
