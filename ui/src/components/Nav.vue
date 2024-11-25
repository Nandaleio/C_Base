<script setup lang="ts">

interface Props {
    list: {
        label?: string
        icon?: string
        path?: string
        separator?: boolean
    }[],
}

    import router from '@/router';
    const props = defineProps<Props>();
</script>

<template>
  <div class="nav">
    <template v-for="item in props.list">

        <span v-if="item.separator" class="separator"></span>

        <button v-else @click="router.push(item.path || '/')">

            <span v-if="item.icon" class="material-symbols-outlined">
                {{ item.icon }}
            </span>

            <template v-else>{{ item.label }}</template>

        </button>
    </template>
  </div>
</template>

<style>
.nav {
    display: flex;
    flex-direction: column;
    gap: 1rem;

    background-color: var(--container-secondary-color);
    height: 100%;
    padding: 1rem;
}

.separator{
    flex-grow: 1;
}

</style>