
<script setup lang="ts">
import router from '@/router';
import { env } from '@/utils/env';
import { ref } from 'vue'

  const username = ref('');
  const password = ref('');

  async function login() {
    const res = await fetch(`${env.apiURL}/api/auth/login`, {
      method: 'post',
      body: JSON.stringify({username: username.value, password: password.value})
    })
    if(res.ok) {
      const {token} = await res.json();
      localStorage.setItem(env.localStorageTokenKey, token);
      router.push('/tables')
    }
  }

</script>

<template>
  <div class="login-form">
      <h1>C_Base Admin Login</h1>
      <input type="text" v-model="username" placeholder="Username">
      <input type="password" v-model="password" placeholder="Password">
      <button class="primary" @click="login">Login</button>
  </div>
</template>

<style scoped>
.login-form {
  width: 100%;
  height: 100%;

  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;

  input{
    width: 20%;
  }
}
</style>