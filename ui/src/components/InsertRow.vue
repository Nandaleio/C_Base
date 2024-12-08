<script setup lang="ts">

import {
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogClose
} from '@/components/ui/dialog'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';

    import { cbFetch } from '@/services/api-service';
    import { ref } from 'vue';

    defineProps<{
        tableName: string
        cols: any[]
    }>()

    const error = ref('');

    async function insertRow() {
       
    }
  
    const close = () => {error.value="";}
</script>

<template>

<DialogContent>
        <DialogHeader>
            <DialogTitle>Insert a new row into {{ tableName }}</DialogTitle>
            <DialogDescription>
                Every admin created will automatically have access to the admin UI
            </DialogDescription>
        </DialogHeader>

        <Input v-for="col of cols" type="" :placeholder="col"/>
        
        <pre v-if="error" class="text-destructive">{{ error }}</pre>

        <DialogFooter>
            <DialogClose as-child>
                <Button @click="close" type="button" variant="secondary">
                    Cancel
                </Button>
            </DialogClose>
            <Button @click="insertRow">Confirm</Button>
        </DialogFooter>
    </DialogContent>
</template>