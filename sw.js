const cache_name = 'vc64_app_cache_v2021_04_06b';

// install event
self.addEventListener('install', evt => {
  console.log('service worker installed');
});

// activate event
self.addEventListener('activate', evt => {
  console.log('service worker activated');
  evt.waitUntil(
    caches.keys().then(keys => {
      console.log('deleting cache files:'+keys);
      return Promise.all(keys
        .map(key => caches.delete(key))
      );
    })
  );
});

// fetch event
self.addEventListener('fetch', evt => {
  //console.log('fetch event', evt);
  evt.respondWith(
    caches.match(evt.request).then(cache_res => {
      return cache_res || fetch(evt.request).then(fetch_res => {
        return caches.open(cache_name).then(cache => {
          if(
            evt.request.url.startsWith('https://csdb.dk/webservice/') && 
            !evt.request.url.endsWith('cache_me=true')
            ||
            evt.request.url.startsWith('https://mega65.github.io/')
	          ||
            evt.request.url.startsWith('https://dirkwhoffmann.github.io/virtualc64web/doc')
            ||
            evt.request.url.endsWith('vc64web_player.js')
          )
          {
            console.log('do not cache fetched resource: '+evt.request.url);
          }
          else
          {
            console.log('into '+cache_name+' putting fetched resource: '+evt.request.url);
            cache.put(evt.request.url, fetch_res.clone());
          }
          return fetch_res;
        })
      });
    }).catch(() => {
        console.log('sw: can not load '+evt.request.url+' from server.');
    })
  );
});

//to test in chrome chrome://flags/#unsafely-treat-insecure-origin-as-secure
