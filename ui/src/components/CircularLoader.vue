<script setup lang="ts">
</script>

<template>
    <div></div>
</template>
  
  
<style scoped>
  div {
    size: 50px;
    color: var(--primary-color);
    border-width: 5px;
    animation-duration: 1s;

    display: inline-block;
    border-style: solid;
    border-radius: 50%;
    animation: spin infinite linear;
  }
  
  @keyframes spin {
    0% {
      transform: rotate(0deg);
    }
    100% {
      transform: rotate(360deg);
    }
  }
</style>
  