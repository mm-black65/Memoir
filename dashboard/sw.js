// sw.js - Service Worker for Memoir PWA
// Bump this string on every deploy so old cached UI never gets stuck.
const CACHE_NAME = 'memoir-v2';
const urlsToCache = [
  '/',
  '/index.html',
  '/manifest.json'
];

self.addEventListener('install', event => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then(cache => cache.addAll(urlsToCache))
      .then(() => self.skipWaiting())
  );
});

self.addEventListener('activate', event => {
  event.waitUntil(
    caches.keys().then(cacheNames => {
      return Promise.all(
        cacheNames.filter(name => name !== CACHE_NAME)
          .map(name => caches.delete(name))
      );
    }).then(() => self.clients.claim())
  );
});

// Network-first for navigations/HTML so UI edits (sidebar, layout, etc.)
// always show up immediately. Cache is only a fallback for offline use.
self.addEventListener('fetch', event => {
  const req = event.request;
  const isHTML = req.mode === 'navigate' || req.destination === 'document';

  if (isHTML) {
    event.respondWith(
      fetch(req)
        .then(res => {
          const clone = res.clone();
          caches.open(CACHE_NAME).then(cache => cache.put(req, clone));
          return res;
        })
        .catch(() => caches.match(req).then(res => res || caches.match('/index.html')))
    );
    return;
  }

  // Cache-first for static assets (icons, fonts, manifest, etc.)
  event.respondWith(
    caches.match(req).then(response => {
      if (response) return response;
      return fetch(req).then(res => {
        const clone = res.clone();
        caches.open(CACHE_NAME).then(cache => cache.put(req, clone));
        return res;
      });
    })
  );
});