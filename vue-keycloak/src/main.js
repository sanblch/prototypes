import Vue from 'vue'
import App from './App.vue'
import Keycloak from 'keycloak-js'

Vue.config.productionTip = false

let initOptions = {
  url: 'http://10.90.137.182:8080/auth', realm: 'vue', clientId: 'vue-test', onLoad:'login-required'
}

let keycloak = Keycloak(initOptions);

keycloak.init({ onLoad: initOptions.onLoad }).success((auth) =>{
    
    if(!auth) {
      window.location.reload();
    }
 
    new Vue({
      render: h => h(App),
    }).$mount('#app')

    localStorage.setItem("vue-token", keycloak.token);
    localStorage.setItem("vue-refresh-token", keycloak.refreshToken);

    setInterval(() =>{
      keycloak.updateToken(70).success((refreshed)=>{
        if (refreshed) {
          Vue.$log.debug('Token refreshed'+ refreshed);
        } else {
          Vue.$log.warn('Token not refreshed, valid for '
          + Math.round(keycloak.tokenParsed.exp + keycloak.timeSkew - new Date().getTime() / 1000) + ' seconds');
        }
      }).error(()=>{
          Vue.$log.error('Failed to refresh token');
      });


    }, 60000)

}).error(() =>{
  Vue.$log.error("Authenticated Failed");
});
