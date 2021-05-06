# vue-keycloak
Based on [this](https://medium.com/keycloak/secure-vue-js-app-with-keycloak-94814181e344)
## Env setup
```
npm install -g @vue/cli
npm cache clear --force
vue create vue-keycloak
cd vue-keycloak
npm i keycloak-js --save
```

## Keycloak setup
- Create realm `vue`
- Create client `vue-test`
  - Root URL: `http://localhost:8000/*`
  - Valid Redirect URIs: `http://localhost:8000/*`
- Realm Settings -> Login -> User registration -> `ON`


## Project setup
```
npm install
```

### Compiles and hot-reloads for development
```
npm run serve -- --port 8000
```

### Compiles and minifies for production
```
npm run build
```

### Lints and fixes files
```
npm run lint
```

### Customize configuration
See [Configuration Reference](https://cli.vuejs.org/config/).
