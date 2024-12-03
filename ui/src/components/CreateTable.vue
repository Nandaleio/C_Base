<script setup lang="ts">
import { useTemplateRef } from 'vue';


const dialog = useTemplateRef('dialog')




const open = () => {if(dialog.value) dialog.value.open = true;}
const close = () => {if(dialog.value) dialog.value.open = false;}
defineExpose({open, close});
</script>

<template>
    <dialog ref="dialog">
        <article>
            <header>
                <h2>Create a new table</h2>
            </header>
            
            <footer>
                <button class="secondary" @click="close">Cancel</button>
                <button>Confirm</button>
            </footer>
        </article>
    </dialog>
</template>

<style scoped>
</style>