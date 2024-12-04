<script setup lang="ts">
import { ref, useTemplateRef } from 'vue';



const tableName = ref<string>('');
const column = ref<{name:string, type:string}[]>([]);


const dialog = useTemplateRef('dialog')
const open = () => {
    if(dialog.value) {
        dialog.value.open = true;
    }
}
const close = () => {
    if(dialog.value) {
        dialog.value.open = false;
    }
    column.value = [];
    tableName.value = '';
}

defineExpose({open, close});
</script>

<template>
    <dialog ref="dialog">
        <article>
            <header>
                <h2>Create a new table</h2>
            </header>
            <input type="text" name="tableName" placeholder="Table Name" v-model="tableName">

            <div role="group" v-for="col of column">
                <input type="text" name="text" placeholder="Column Name" v-model="col.name">
                <select name="columnName" aria-label="Select the column type..." required v-model="col.type">
                    <option selected disabled value="">
                        Select the column type...
                    </option>
                    <option value="text">TEXT</option>
                    <option value="real">REAL</option>
                    <option value="integer">INTEGER</option>
                    <option value="nulll">NULL</option>
                    <option value="file" disabled>FILE</option>
                    <option value="relation" disabled>RELATION</option>
                </select>
                <button class="delete-btn secondary" @click="column.splice(column.indexOf(col), 1)">
                    <span class="material-symbols-outlined">delete</span>
                </button>
            </div>
            
            <button @click="column.push({name:'', type:''})">Add Column</button>

            
            <footer>
                <button class="secondary" @click="close">Cancel</button>
                <button>Confirm</button>
            </footer>
        </article>
    </dialog>
</template>

<style scoped>
.delete-btn {
    display: flex;
}
</style>